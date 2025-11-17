// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSObjectTickFunction.h"

FVSObjectTickFunction::~FVSObjectTickFunction()
{
	if (IsTickFunctionRegistered())
	{
		UnregisterAndCleanup();
	}
	
	if (OnWorldInitializeDelegateHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(OnWorldInitializeDelegateHandle);
	}
	if (OnWorldTearDownDelegateHandle.IsValid())
	{
		FWorldDelegates::OnWorldBeginTearDown.Remove(OnWorldTearDownDelegateHandle);
	}
	
	Target.Reset();
	TypedTargetOuter.Reset();

	if (OnExecuteTick.IsBound())
	{
		OnExecuteTick.Unbind();
	}
}

void FVSObjectTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (!Target.IsValid() || CanExecuteTick.IsBound() && !CanExecuteTick.Execute()) return;
	if (!OnExecuteTick.IsBound()) return;

	bool bShouldExecuteTick = false;
	float TimeDilation = 1.f;
	if (TypedTargetOuter.HasSubtype<TWeakObjectPtr<UActorComponent>>())
	{
		if (const UActorComponent* ActorComponent = TypedTargetOuter.GetSubtype<TWeakObjectPtr<UActorComponent>>().Get())
		{
			if (TickType != LEVELTICK_ViewportsOnly
				|| (ActorComponent->bTickInEditor && TickType == LEVELTICK_ViewportsOnly)
				|| (ActorComponent->GetOwner() && ActorComponent->GetOwner()->ShouldTickIfViewportsOnly()))
			{
				if (const AActor* Actor = ActorComponent->GetOwner())
				{
					TimeDilation = Actor->CustomTimeDilation;
					bShouldExecuteTick = true;
				}
			}
		}
	}
	else if (TypedTargetOuter.HasSubtype<TWeakObjectPtr<AActor>>())
	{
		if (const AActor* Actor = TypedTargetOuter.GetSubtype<TWeakObjectPtr<AActor>>().Get())
		{
			if (TickType != LEVELTICK_ViewportsOnly || Actor->ShouldTickIfViewportsOnly())
			{
				TimeDilation = Actor->CustomTimeDilation;
				bShouldExecuteTick = true;
			}
		}
	}
	else if (TypedTargetOuter.HasSubtype<TWeakObjectPtr<UObject>>())
	{
		if (const UObject* Object = TypedTargetOuter.GetSubtype<TWeakObjectPtr<UObject>>().Get())
		{
			bShouldExecuteTick = true;
		}
	}
	
	if (bShouldExecuteTick && OnExecuteTick.IsBound())
	{
		FScopeCycleCounterUObject TargetScope(Target.Get());
		OnExecuteTick.ExecuteIfBound(DeltaTime * TimeDilation, TickType, this);
	}
}

FString FVSObjectTickFunction::DiagnosticMessage()
{
	return (Target.IsValid() ? Target->GetFullName() : "Nullptr") + TEXT("[TickObject]");
}

FName FVSObjectTickFunction::DiagnosticContext(bool bDetailed)
{
	if (bDetailed)
	{
		/** Format is "AssociationNativeClass/AssociationClass". */
		FString ContextString = FString::Printf(TEXT("%s/%s"), *GetParentNativeClass(Target->GetClass())->GetName(), *Target->GetClass()->GetName());
		return FName(*ContextString);
	}
	return GetParentNativeClass(Target->GetClass())->GetFName();
}

void FVSObjectTickFunction::RegisterAndSetup(UObject* InTargetObject)
{
	if (!bCanEverTick || IsTickFunctionRegistered()) return;
	if (!InTargetObject || InTargetObject->IsUnreachable() || InTargetObject->IsTemplate()) return;

	if (bTickCrossWorldIfPossible && !IsWorldRelatedObject(InTargetObject) /** && is in game */)
	{
		bShouldTickCrossWorld = true;
		OnWorldInitializeDelegateHandle = FWorldDelegates::OnPostWorldInitialization.AddLambda([this] (UWorld* InWorld, FWorldInitializationValues)
		{
			if (!InWorld || !InWorld->GetCurrentLevel()) return;
			UnRegisterTickFunction();
			RegisterTickFunction(InWorld->GetCurrentLevel());
		});

		OnWorldTearDownDelegateHandle = FWorldDelegates::OnWorldBeginTearDown.AddLambda([this] (UWorld* InWorld)
		{
			UnRegisterTickFunction();
		});

		Target = InTargetObject;
	}
	
	UWorld* World = InTargetObject->GetWorld();
	if (!World)
	{
		if (IsInGameThread())
		{
			World = GWorld->GetWorld();
		}
	}
	if (!World) return;

	Target = InTargetObject;

	/** Find the nearest actor component or actor in the outer chain. */
	if (UActorComponent* ActorComponent = Target->GetTypedOuter<UActorComponent>())
	{
		TypedTargetOuter.SetSubtype<TWeakObjectPtr<UActorComponent>>(ActorComponent);
	}
	else if (AActor* Actor = Target->GetTypedOuter<AActor>())
	{
		TypedTargetOuter.SetSubtype<TWeakObjectPtr<AActor>>(Actor);
	}
	else
	{
		TypedTargetOuter.SetSubtype<TWeakObjectPtr<UObject>>(Target);
	}

	SetTickFunctionEnable(bStartWithTickEnabled || IsTickFunctionEnabled());
	if (ULevel* CurrentLevel = World->GetCurrentLevel())
	{
		RegisterTickFunction(CurrentLevel);
	}
}

void FVSObjectTickFunction::UnregisterAndCleanup()
{
	if (!IsTickFunctionRegistered()) return;
	
	SetTickFunctionEnable(false);
	UnRegisterTickFunction();

	if (CanExecuteTick.IsBound()) { CanExecuteTick.Unbind(); }
	if (OnExecuteTick.IsBound()) { OnExecuteTick.Unbind(); }

	Target.Reset();
	TypedTargetOuter.Reset();

	if (OnWorldInitializeDelegateHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(OnWorldInitializeDelegateHandle);
	}
	if (OnWorldTearDownDelegateHandle.IsValid())
	{
		FWorldDelegates::OnWorldBeginTearDown.Remove(OnWorldTearDownDelegateHandle);
	}

	bShouldTickCrossWorld = false;
}

bool FVSObjectTickFunction::IsWorldRelatedObject(const UObject* Object)
{
	if (!Object) return false;
	if (Object->IsA<AActor>() || Object->IsA<UActorComponent>() || Object->IsA<UWorldSubsystem>()) return true;
	
	return IsWorldRelatedObject(Object->GetOuter());
}
