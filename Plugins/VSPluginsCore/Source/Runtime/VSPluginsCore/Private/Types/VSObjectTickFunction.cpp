// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSObjectTickFunction.h"

FVSObjectTickFunction::~FVSObjectTickFunction()
{
	Target.Reset();
	TypedOuter.Reset();

	if (OnExecuteTick.IsBound())
	{
		OnExecuteTick.Unbind();
	}
}

void FVSObjectTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (CanExecuteTick.IsBound() && !CanExecuteTick.Execute()) return;
	if (!OnExecuteTick.IsBound()) return;

	bool bShouldExecuteTick = false;
	float TimeDilation = 1.f;
	if (TypedOuter.HasSubtype<TWeakObjectPtr<UActorComponent>>())
	{
		if (const UActorComponent* ActorComponent = TypedOuter.GetSubtype<TWeakObjectPtr<UActorComponent>>().Get())
		{
			FScopeCycleCounterUObject ComponentScope(ActorComponent);
			FScopeCycleCounterUObject AdditionalScope(ActorComponent->AdditionalStatObject());
			
			const AActor* Actor = ActorComponent->GetOwner();
			if (TickType != LEVELTICK_ViewportsOnly
				|| (ActorComponent->bTickInEditor && TickType == LEVELTICK_ViewportsOnly)
				|| (ActorComponent->GetOwner() && ActorComponent->GetOwner()->ShouldTickIfViewportsOnly()))
			{
				TimeDilation = Actor->CustomTimeDilation;
				bShouldExecuteTick = true;
			}
		}
	}
	else if (TypedOuter.HasSubtype<TWeakObjectPtr<AActor>>())
	{
		if (const AActor* Actor = TypedOuter.GetSubtype<TWeakObjectPtr<AActor>>().Get())
		{
			if (TickType != LEVELTICK_ViewportsOnly || Actor->ShouldTickIfViewportsOnly())
			{
				FScopeCycleCounterUObject ActorScope(Actor);
				TimeDilation = Actor->CustomTimeDilation;
				bShouldExecuteTick = true;
			}
		}
	}
	else if (TypedOuter.HasSubtype<TWeakObjectPtr<UObject>>())
	{
		if (const UObject* Object = TypedOuter.GetSubtype<TWeakObjectPtr<UObject>>().Get())
		{
			FScopeCycleCounterUObject ObjectScope(Object);
			bShouldExecuteTick = true;
		}
	}
	
	if (bShouldExecuteTick)
	{
		OnExecuteTick.ExecuteIfBound(DeltaTime * TimeDilation, TickType, this);
	}
}

FString FVSObjectTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[TickObject]");
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

	/** Check the InTargetObject's validation. */
	if (!InTargetObject || InTargetObject->IsUnreachable() || InTargetObject->IsTemplate()) return;
	UWorld* World = InTargetObject->GetWorld();
	if (!World) return;

	Target = InTargetObject;
	
	/** Find an actor component and an actor in the outer chain. */
	if (UActorComponent* ActorComponent = InTargetObject->GetTypedOuter<UActorComponent>())
	{
		TypedOuter.SetSubtype<TWeakObjectPtr<UActorComponent>>(ActorComponent);
	}
	else if (AActor* Actor = InTargetObject->GetTypedOuter<AActor>())
	{
		TypedOuter.SetSubtype<TWeakObjectPtr<AActor>>(Actor);
	}
	else
	{
		TypedOuter.SetSubtype<TWeakObjectPtr<UObject>>(InTargetObject);
	}
	
	SetTickFunctionEnable(bStartWithTickEnabled || IsTickFunctionEnabled());
	RegisterTickFunction(World->GetCurrentLevel());
}

void FVSObjectTickFunction::UnregisterAndCleanup()
{
	UnRegisterTickFunction();

	if (CanExecuteTick.IsBound()) { CanExecuteTick.Unbind(); }
	if (OnExecuteTick.IsBound()) { OnExecuteTick.Unbind(); }

	Target.Reset();
	TypedOuter.Reset();
}
