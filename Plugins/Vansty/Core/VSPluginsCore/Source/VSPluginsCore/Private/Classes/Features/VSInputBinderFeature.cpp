// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSInputBinderFeature.h"
#include "EnhancedInputComponent.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSActorLibrary.h"

UVSInputActionBinderFeature::UVSInputActionBinderFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInputActionBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ControllerPrivate = UVSActorLibrary::GetControllerFromActor(GetOwnerActor());
	if (!ControllerPrivate.IsValid()) return;

	GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
	if (!GameplayTagFeaturePrivate.IsValid()) return;

	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		GameplayTagFeaturePrivate->BindDelegateForObject(this);
	}
}

void UVSInputActionBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;
	
	/** Bind default actions. */
	for (const auto& Action : ActionSettings)
	{
		if (Action.bBindByDefault)
		{
			BindActionSettings(Action);
		}
	}

	RefreshActions(FGameplayTagContainer::EmptyContainer);
}

void UVSInputActionBinderFeature::Uninitialize_Implementation()
{
	/** Unbind all actions. */
	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		for (const auto& Action : ActionSettings)
		{
			UnbindActionSettings(Action);
		}
	}
	
	if (GameplayTagFeaturePrivate.IsValid())
	{
		GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
	}
	
	GameplayTagFeaturePrivate.Reset();
	ControllerPrivate.Reset();

	Super::Uninitialize_Implementation();
}

UInputComponent* UVSInputActionBinderFeature::GetInputComponent() const
{
	return ControllerPrivate.IsValid() ? ControllerPrivate->InputComponent : nullptr;
}

void UVSInputActionBinderFeature::BindActionSettings(const FVSInputActionBinderSettings& InActionSettings)
{
	if (InActionSettings.ActionName.IsNone()) return;
	
	UInputComponent* InputComponent = GetInputComponent();
	if (!InputComponent) return;
	
	for (const EInputEvent InputEvent : InActionSettings.InputEvents)
	{
		if (InputEvent == IE_MAX) continue;
		
		FInputActionBinding Binding = FInputActionBinding(InActionSettings.ActionName, InputEvent);
		if (InActionSettings.SpecifiedKey.IsValid())
		{
			Binding.ActionDelegate.GetDelegateWithKeyForManualSet().BindLambda([this, InActionSettings, InputEvent] (FKey Key)
			{
				if (Key == InActionSettings.SpecifiedKey)
				{
					HandleActionTriggered(InActionSettings.ActionName, InputEvent);
				}
			});
		}
		else
		{
			Binding.ActionDelegate.GetDelegateForManualSet().BindLambda([this, InActionSettings, InputEvent]()
			{
				HandleActionTriggered(InActionSettings.ActionName, InputEvent);
			});
		}
		
		InputComponent->AddActionBinding(Binding);
	}
}

void UVSInputActionBinderFeature::UnbindActionSettings(const FVSInputActionBinderSettings& InActionSettings)
{
	if (InActionSettings.ActionName.IsNone() || InActionSettings.InputEvents.IsEmpty()) return;
	
	UInputComponent* InputComponent = GetInputComponent();
	if (!InputComponent) return;
	
	for (const TEnumAsByte<EInputEvent> InputEvent : InActionSettings.InputEvents)
	{
		if (InputEvent == IE_MAX) continue;
		InputComponent->RemoveActionBinding(InActionSettings.ActionName, InputEvent);
	}
}

void UVSInputActionBinderFeature::RefreshActions(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (const auto& Action : ActionSettings)
	{
		if (Action.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			BindActionSettings(Action);
		}
		if (Action.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			UnbindActionSettings(Action);
		}
	}
}

void UVSInputActionBinderFeature::HandleActionTriggered(FName ActionName, EInputEvent InputEvent)
{
	OnActionTriggered_Native.Broadcast(this, ActionName, InputEvent);
	OnActionTriggered.Broadcast(this, ActionName, InputEvent);
}

UVSGameplayTagFeatureBase* UVSInputActionBinderFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
}

void UVSInputActionBinderFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshActions(FGameplayTagContainer::EmptyContainer);
}

void UVSInputActionBinderFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshActions(TagEvents);
}


/** ------------------------------------------------------------------------- **/

void UVSInputAxisBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ControllerPrivate = UVSActorLibrary::GetControllerFromActor(GetOwnerActor());
	if (!ControllerPrivate.IsValid()) return;

	GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
	if (!GameplayTagFeaturePrivate.IsValid()) return;

	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		GameplayTagFeaturePrivate->BindDelegateForObject(this);
	}
}

void UVSInputAxisBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;

	/** Bind default actions. */
	for (const auto& Axis : AxisSettings)
	{
		if (Axis.bBindByDefault)
		{
			BindAxisSettings(Axis);
		}
	}

	RefreshAxis(FGameplayTagContainer::EmptyContainer);
}

void UVSInputAxisBinderFeature::Uninitialize_Implementation()
{
	/** Unbind all axes. */
	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		for (const auto& Axis : AxisSettings)
		{
			UnbindAxisSettings(Axis);
		}
	}
	
	if (GameplayTagFeaturePrivate.IsValid())
	{
		GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
	}
	
	GameplayTagFeaturePrivate.Reset();
	ControllerPrivate.Reset();

	
	Super::Uninitialize_Implementation();
}

UVSGameplayTagFeatureBase* UVSInputAxisBinderFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
}

void UVSInputAxisBinderFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshAxis(FGameplayTagContainer::EmptyContainer);
}

void UVSInputAxisBinderFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshAxis(TagEvents);
}

UInputComponent* UVSInputAxisBinderFeature::GetInputComponent() const
{
	return ControllerPrivate.IsValid() ? ControllerPrivate->InputComponent : nullptr;
}

void UVSInputAxisBinderFeature::BindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings)
{
	if (InActionSettings.AxisName.IsNone()) return;
	
	UInputComponent* InputComponent = GetInputComponent();
	if (!InputComponent) return;

	if (InActionSettings.SpecifiedKey.IsValid())
	{
		FInputAxisKeyBinding AxisKeyBinding(InActionSettings.SpecifiedKey);
		AxisKeyBinding.AxisDelegate.GetDelegateForManualSet().BindLambda([this, InActionSettings] (const float Value)
		{
			HandleAxisTriggered(InActionSettings.AxisName, Value);
		});
		
		InputComponent->AxisKeyBindings.Emplace(MoveTemp(AxisKeyBinding));
	}
	else
	{
		FInputAxisBinding AxisBinding(InActionSettings.AxisName);
		AxisBinding.AxisDelegate.GetDelegateForManualSet().BindLambda([this, InActionSettings] (const float Value)
		{
			HandleAxisTriggered(InActionSettings.AxisName, Value);
		});
		
		InputComponent->AxisBindings.Emplace(MoveTemp(AxisBinding));
	}
}

void UVSInputAxisBinderFeature::UnbindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings)
{
	if (InActionSettings.AxisName.IsNone()) return;
	
	UInputComponent* InputComponent = GetInputComponent();
	if (!InputComponent) return;

	if (InActionSettings.SpecifiedKey.IsValid())
	{
		InputComponent->AxisKeyBindings.Remove(InActionSettings.SpecifiedKey);
	}
	else
	{
		InputComponent->RemoveAxisBinding(InActionSettings.AxisName);
	}
}

void UVSInputAxisBinderFeature::RefreshAxis(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (const auto& Axis : AxisSettings)
	{
		if (Axis.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			BindAxisSettings(Axis);
		}
		if (Axis.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			UnbindAxisSettings(Axis);
		}
	}
}

void UVSInputAxisBinderFeature::HandleAxisTriggered(FName AxisName, float Value)
{
	OnAxisExecuted_Native.Broadcast(this, AxisName, Value);
	OnAxisTriggered.Broadcast(this, AxisName, Value);
}


/** ------------------------------------------------------------------------- **/


UVSEnhancedInputActionBinderFeature::UVSEnhancedInputActionBinderFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSEnhancedInputActionBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (!UVSActorLibrary::IsActorNetLocal(GetOwnerActor())) return;

	ControllerPrivate = UVSActorLibrary::GetControllerFromActor(GetOwnerActor());
	if (!ControllerPrivate.IsValid()) return;

	GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
	if (!GameplayTagFeaturePrivate.IsValid()) return;

	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		GameplayTagFeaturePrivate->BindDelegateForObject(this);
	}
}

void UVSEnhancedInputActionBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	/** Bind default actions. */
	for (const auto& Action : ActionSettings)
	{
		if (Action.bBindByDefault)
		{
			BindActionSettings(Action);
		}
	}

	RefreshActions(FGameplayTagContainer::EmptyContainer);
}

void UVSEnhancedInputActionBinderFeature::Uninitialize_Implementation()
{
	/** Unbind all actions. */
	if (UVSActorLibrary::IsActorNetLocal(GetOwnerActor()))
	{
		for (const auto& Action : ActionSettings)
		{
			UnbindActionSettings(Action);
		}
	}
	
	if (GameplayTagFeaturePrivate.IsValid())
	{
		GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
	}
	
	GameplayTagFeaturePrivate.Reset();
	ControllerPrivate.Reset();
	
	Super::Uninitialize_Implementation();
}

UVSGameplayTagFeatureBase* UVSEnhancedInputActionBinderFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
}

void UVSEnhancedInputActionBinderFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshActions(FGameplayTagContainer::EmptyContainer);
}

void UVSEnhancedInputActionBinderFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshActions(TagEvents);
}

UEnhancedInputComponent* UVSEnhancedInputActionBinderFeature::GetEnhancedInputComponent() const
{
	return ControllerPrivate.IsValid() ? Cast<UEnhancedInputComponent>(ControllerPrivate->InputComponent) : nullptr;
}

void UVSEnhancedInputActionBinderFeature::BindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings)
{
	if (InActionSettings.ActionName.IsNone() || !InActionSettings.InputAction || InActionSettings.TriggerEvents.IsEmpty()) return;
	
	UEnhancedInputComponent* InputComponent = GetEnhancedInputComponent();
	if (!InputComponent) return;

	for (const ETriggerEvent TriggerEvent : InActionSettings.TriggerEvents)
	{
		if (TriggerEvent == ETriggerEvent::None) continue;
		
		InputComponent->BindActionInstanceLambda(InActionSettings.InputAction, TriggerEvent,
			[this, InActionSettings, TriggerEvent] (const FInputActionInstance& ActionInstance)
		{
			HandleActionExecution(InActionSettings.ActionName, TriggerEvent, ActionInstance);
		});
	}
}

void UVSEnhancedInputActionBinderFeature::UnbindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings)
{
	UEnhancedInputComponent* InputComponent = GetEnhancedInputComponent();
	if (!InputComponent) return;

	TArray<uint32> HandlesToRemove;

	for (const auto& BindingPtr : InputComponent->GetActionEventBindings())
	{
		if (!BindingPtr || BindingPtr->GetAction() != InActionSettings.InputAction) continue;
		
		for (ETriggerEvent TriggerEvent : InActionSettings.TriggerEvents)
		{
			if (TriggerEvent == ETriggerEvent::None) continue;

			if (BindingPtr->GetTriggerEvent() == TriggerEvent)
			{
				HandlesToRemove.Add(BindingPtr->GetHandle());
				break;
			}
		}
	}

	for (uint32 Handle : HandlesToRemove)
	{
		InputComponent->RemoveBindingByHandle(Handle);
	}
}

void UVSEnhancedInputActionBinderFeature::RefreshActions(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (const auto& Action : ActionSettings)
	{
		if (Action.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			BindActionSettings(Action);
		}
		if (Action.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			UnbindActionSettings(Action);
		}
	}
}

void UVSEnhancedInputActionBinderFeature::HandleActionExecution(FName ActionName, ETriggerEvent TriggerEvent, const FInputActionInstance& ActionInstance)
{
	OnActionTriggered_Native.Broadcast(this, ActionName, TriggerEvent, ActionInstance);
	OnActionTriggered.Broadcast(this, ActionName, TriggerEvent, ActionInstance);
}
