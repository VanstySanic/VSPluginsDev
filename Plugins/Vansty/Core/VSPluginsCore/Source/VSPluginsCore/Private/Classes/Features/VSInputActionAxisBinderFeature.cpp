// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Features/VSInputActionAxisBinderFeature.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSActorLibrary.h"

namespace
{
	uint64 MakeActionEventKey(FName ActionName, EInputEvent InputEvent)
	{
		const uint32 A = GetTypeHash(ActionName);
		const uint32 B = GetTypeHash(static_cast<uint8>(InputEvent));
		return (uint64(A) << 32) | uint64(B);
	}

	uint64 MakeEnhancedBindingKey(const UInputAction* InputAction, ETriggerEvent TriggerEvent)
	{
		const uint32 A = GetTypeHash(InputAction);
		const uint32 B = GetTypeHash(static_cast<uint8>(TriggerEvent));
		return (uint64(A) << 32) | uint64(B);
	}

	bool HasActionBinding(const UInputComponent* InputComponent, const FName ActionName, const EInputEvent InputEvent)
	{
		if (!InputComponent)
		{
			return false;
		}

		const int32 NumBindings = InputComponent->GetNumActionBindings();
		for (int32 Index = 0; Index < NumBindings; Index++)
		{
			const FInputActionBinding& ExistingBinding = InputComponent->GetActionBinding(Index);
			if (ExistingBinding.GetActionName() == ActionName && ExistingBinding.KeyEvent == InputEvent)
			{
				return true;
			}
		}

		return false;
	}

	bool HasAxisBinding(const UInputComponent* InputComponent, const FName AxisName)
	{
		if (!InputComponent)
		{
			return false;
		}

		for (const FInputAxisBinding& Binding : InputComponent->AxisBindings)
		{
			if (Binding.AxisName == AxisName)
			{
				return true;
			}
		}

		return false;
	}

	bool HasAxisKeyBinding(const UInputComponent* InputComponent, const FKey Key)
	{
		if (!InputComponent)
		{
			return false;
		}

		for (const FInputAxisKeyBinding& Binding : InputComponent->AxisKeyBindings)
		{
			if (Binding.AxisKey == Key)
			{
				return true;
			}
		}

		return false;
	}

	bool HasEnhancedBindingHandle(const UEnhancedInputComponent* InputComponent, const uint32 Handle)
	{
		if (!InputComponent) return false;
		
		for (const TUniquePtr<FEnhancedInputActionEventBinding>& BindingPtr : InputComponent->GetActionEventBindings())
		{
			if (BindingPtr && BindingPtr->GetHandle() == Handle)
			{
				return true;
			}
		}

		return false;
	}
}



UVSInputActionBinderFeature::UVSInputActionBinderFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInputActionBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	APlayerController* PlayerController = Cast<APlayerController>(UVSActorLibrary::GetControllerFromActor(GetOwnerActor()));
	if (PlayerController && PlayerController->IsLocalController())
	{
		FeatureInputComponent = NewObject<UInputComponent>(PlayerController);
		if (FeatureInputComponent.IsValid())
		{
			FeatureInputComponent->Priority = InputPriority;
			FeatureInputComponent->bBlockInput = bBlockInput;
			FeatureInputComponent->RegisterComponent();
			PlayerController->PushInputComponent(FeatureInputComponent.Get());

			GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
			if (GameplayTagFeaturePrivate.IsValid())
			{
				GameplayTagFeaturePrivate->BindDelegateForObject(this);
			}
		}
	}
}

void UVSInputActionBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	if (FeatureInputComponent.IsValid())
	{
		/** Bind default actions. */
		for (int32 Index = 0; Index < ActionSettings.Num(); ++Index)
		{
			const auto& Action = ActionSettings[Index];
			if (Action.bBindByDefault && !BoundActionSettings.Contains(Index))
			{
				BindActionSettings(Action);
				BoundActionSettings.Add(Index);
			}
		}

		RefreshActions(FGameplayTagContainer::EmptyContainer);
	}
}

void UVSInputActionBinderFeature::Uninitialize_Implementation()
{
	if (FeatureInputComponent.IsValid())
	{
		/** Unbind bound actions only. */
		for (int32 Index : BoundActionSettings)
		{
			if (ActionSettings.IsValidIndex(Index))
			{
				UnbindActionSettings(ActionSettings[Index]);
			}
		}

		if (GameplayTagFeaturePrivate.IsValid())
		{
			GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(FeatureInputComponent->GetOwner()))
		{
			PlayerController->PopInputComponent(FeatureInputComponent.Get());
		}
		FeatureInputComponent->DestroyComponent();
		
		BoundActionSettings.Reset();
		GameplayTagFeaturePrivate.Reset();
		FeatureInputComponent.Reset();
		ActionKeyMap.Empty();
		ActionEventAnyKey.Empty();
		ActionEventBoundKeys.Empty();
	}

	Super::Uninitialize_Implementation();
}

UInputComponent* UVSInputActionBinderFeature::GetInputComponent() const
{
	return FeatureInputComponent.IsValid() ? FeatureInputComponent.Get() : nullptr;
}

void UVSInputActionBinderFeature::BindActionSettings(const FVSInputActionBinderSettings& InActionSettings)
{
	if (!FeatureInputComponent.IsValid() || !InActionSettings.IsValid()) return;
	
	for (const EInputEvent InputEvent : InActionSettings.InputEvents)
	{
		if (InputEvent == IE_MAX) continue;
		const uint64 EventKey = MakeActionEventKey(InActionSettings.ActionName, InputEvent);

		if (!InActionSettings.SpecifiedKeys.IsEmpty())
		{
			for (const FInputChord& Key : InActionSettings.SpecifiedKeys)
			{
				if (Key.Key.IsValid())
				{
					ActionKeyMap.Add(EventKey, Key);
				}
			}
		}
		else
		{
			++ActionEventAnyKey.FindOrAdd(EventKey);
		}
		
		/** Check duplicated binding. */
		if (ActionEventBoundKeys.Contains(EventKey))
		{
			if (HasActionBinding(FeatureInputComponent.Get(), InActionSettings.ActionName, InputEvent))
			{
				continue;
			}

			ActionEventBoundKeys.Remove(EventKey);
		}

		/** Keep single action binding in the input component, which can trigger multiple events. */
		const FName ActionName = InActionSettings.ActionName;
		FInputActionBinding Binding = FInputActionBinding(ActionName, InputEvent);
		Binding.ActionDelegate.GetDelegateWithKeyForManualSet().BindWeakLambda(this, [this, ActionName, EventKey, InputEvent] (FKey Key)
		{
			const int32 AnyCount = ActionEventAnyKey.FindRef(EventKey);
			for (int32 Index = 0; Index < AnyCount; ++Index)
			{
				HandleActionTriggered(ActionName, InputEvent);
			}

			TArray<FInputChord> KeyEntries;
			ActionKeyMap.MultiFind(EventKey, KeyEntries);
			for (const FInputChord& BoundKey : KeyEntries)
			{
				if (BoundKey == Key)
				{
					HandleActionTriggered(ActionName, InputEvent);
				}
			}
		});
		
		FeatureInputComponent->AddActionBinding(Binding);
		ActionEventBoundKeys.Add(EventKey);
	}
}

void UVSInputActionBinderFeature::UnbindActionSettings(const FVSInputActionBinderSettings& InActionSettings)
{
	if (!FeatureInputComponent.IsValid() || !InActionSettings.IsValid()) return;
	
	for (const EInputEvent InputEvent : InActionSettings.InputEvents)
	{
		if (InputEvent == IE_MAX) continue;
		const uint64 EventKey = MakeActionEventKey(InActionSettings.ActionName, InputEvent);

		if (!InActionSettings.SpecifiedKeys.IsEmpty())
		{
			for (const FInputChord& Key : InActionSettings.SpecifiedKeys)
			{
				ActionKeyMap.RemoveSingle(EventKey, Key);
			}
		}
		else
		{
			if (int32* Count = ActionEventAnyKey.Find(EventKey))
			{
				--(*Count);
				if (*Count <= 0)
				{
					ActionEventAnyKey.Remove(EventKey);
				}
			}
		}

		const bool bHasKeys = ActionKeyMap.Contains(EventKey);
		const bool bHasAnyKey = ActionEventAnyKey.Contains(EventKey);
		
		if (bHasKeys || bHasAnyKey) continue;

		/** Remove binding in the input component. */
		ActionEventBoundKeys.Remove(EventKey);
		for (int32 Index = FeatureInputComponent->GetNumActionBindings() - 1; Index >= 0; Index--)
		{
			const FInputActionBinding& ExistingBinding = FeatureInputComponent->GetActionBinding(Index);
			if (ExistingBinding.GetActionName() == InActionSettings.ActionName && ExistingBinding.KeyEvent == InputEvent)
			{
				FeatureInputComponent->RemoveActionBinding(Index);
				break;
			}
		}
	}
}

void UVSInputActionBinderFeature::RefreshActions(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (int32 Index = 0; Index < ActionSettings.Num(); ++Index)
	{
		const auto& Action = ActionSettings[Index];
		if (Action.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (!BoundActionSettings.Contains(Index))
			{
				BindActionSettings(Action);
				BoundActionSettings.Add(Index);
			}
		}
		if (Action.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (BoundActionSettings.Contains(Index))
			{
				UnbindActionSettings(Action);
				BoundActionSettings.Remove(Index);
			}
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


UVSInputAxisBinderFeature::UVSInputAxisBinderFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInputAxisBinderFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	APlayerController* PlayerController = Cast<APlayerController>(UVSActorLibrary::GetControllerFromActor(GetOwnerActor()));
	if (PlayerController && PlayerController->IsLocalController())
	{
		FeatureInputComponent = NewObject<UInputComponent>(PlayerController);
		if (FeatureInputComponent.IsValid())
		{
			FeatureInputComponent->Priority = InputPriority;
			FeatureInputComponent->bBlockInput = bBlockInput;
			FeatureInputComponent->RegisterComponent();
			PlayerController->PushInputComponent(FeatureInputComponent.Get());

			GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
			if (GameplayTagFeaturePrivate.IsValid())
			{
				GameplayTagFeaturePrivate->BindDelegateForObject(this);
			}
		}
	}
}

void UVSInputAxisBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (FeatureInputComponent.IsValid())
	{
		/** Bind default actions. */
		for (int32 Index = 0; Index < AxisSettings.Num(); ++Index)
		{
			const auto& Axis = AxisSettings[Index];
			if (Axis.bBindByDefault && !BoundAxisSettings.Contains(Index))
			{
				BindAxisSettings(Axis);
				BoundAxisSettings.Add(Index);
			}
		}

		RefreshAxes(FGameplayTagContainer::EmptyContainer);
	}
}

void UVSInputAxisBinderFeature::Uninitialize_Implementation()
{
	if (FeatureInputComponent.IsValid())
	{
		/** Unbind bound axes only. */
		for (int32 Index : BoundAxisSettings)
		{
			if (AxisSettings.IsValidIndex(Index))
			{
				UnbindAxisSettings(AxisSettings[Index]);
			}
		}

		if (GameplayTagFeaturePrivate.IsValid())
		{
			GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(FeatureInputComponent->GetOwner()))
		{
			PlayerController->PopInputComponent(FeatureInputComponent.Get());
		}
		FeatureInputComponent->DestroyComponent();
		
		BoundAxisSettings.Reset();
		GameplayTagFeaturePrivate.Reset();
		FeatureInputComponent.Reset();
		AxisKeyNameMap.Empty();
		AxisKeyBoundKeys.Empty();
		AxisNameCounts.Empty();
	}
	
	Super::Uninitialize_Implementation();
}

UVSGameplayTagFeatureBase* UVSInputAxisBinderFeature::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (GameplayTagFeaturePrivate.IsValid()) return GameplayTagFeaturePrivate.Get();
	return IVSGameplayTagFeatureInterface::GetPrimaryGameplayTagFeature_Implementation();
}

void UVSInputAxisBinderFeature::OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature)
{
	RefreshAxes(FGameplayTagContainer::EmptyContainer);
}

void UVSInputAxisBinderFeature::OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents)
{
	RefreshAxes(TagEvents);
}

UInputComponent* UVSInputAxisBinderFeature::GetInputComponent() const
{
	return FeatureInputComponent.IsValid() ? FeatureInputComponent.Get() : nullptr;
}

void UVSInputAxisBinderFeature::BindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings)
{
	if (!FeatureInputComponent.IsValid() || InActionSettings.AxisName.IsNone()) return;
	
	if (!InActionSettings.SpecifiedKeys.IsEmpty())
	{
		for (const FKey& BoundKey : InActionSettings.SpecifiedKeys)
		{
			if (!BoundKey.IsValid()) continue;
			
			if (AxisKeyBoundKeys.Contains(BoundKey) && !HasAxisKeyBinding(FeatureInputComponent.Get(), BoundKey))
			{
				AxisKeyBoundKeys.Remove(BoundKey);
			}

			AxisKeyNameMap.Add(BoundKey, InActionSettings.AxisName);

			if (!AxisKeyBoundKeys.Contains(BoundKey))
			{
				FInputAxisKeyBinding AxisKeyBinding(BoundKey);
				AxisKeyBinding.AxisDelegate.GetDelegateForManualSet().BindWeakLambda(this, [this, BoundKey] (const float Value)
				{
					TArray<FName> AxisNames;
					AxisKeyNameMap.MultiFind(BoundKey, AxisNames);
					for (const FName& AxisName : AxisNames)
					{
						HandleAxisTriggered(AxisName, Value);
					}
				});
				
				FeatureInputComponent->AxisKeyBindings.Emplace(MoveTemp(AxisKeyBinding));
				AxisKeyBoundKeys.Add(BoundKey);
			}
		}
	}
	else
	{
		AxisNameCounts.Add(InActionSettings.AxisName, 0);

		if (HasAxisBinding(FeatureInputComponent.Get(), InActionSettings.AxisName)) return;

		FInputAxisBinding AxisBinding(InActionSettings.AxisName);
		AxisBinding.AxisDelegate.GetDelegateForManualSet().BindWeakLambda(this, [this, InActionSettings] (const float Value)
		{
			TArray<uint8> AxisEntries;
			AxisNameCounts.MultiFind(InActionSettings.AxisName, AxisEntries);
			for (uint8 AxisEntry : AxisEntries)
			{
				HandleAxisTriggered(InActionSettings.AxisName, Value);
			}
		});
		FeatureInputComponent->AxisBindings.Emplace(MoveTemp(AxisBinding));
	}
}

void UVSInputAxisBinderFeature::UnbindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings)
{
	if (InActionSettings.AxisName.IsNone()) return;
	
	UInputComponent* InputComponent = GetInputComponent();
	if (!InputComponent) return;

	if (!InActionSettings.SpecifiedKeys.IsEmpty())
	{
		for (const FKey& BoundKey : InActionSettings.SpecifiedKeys)
		{
			if (!BoundKey.IsValid()) continue;
			
			AxisKeyNameMap.RemoveSingle(BoundKey, InActionSettings.AxisName);
			if (AxisKeyNameMap.Contains(BoundKey)) continue;
			
			InputComponent->AxisKeyBindings.RemoveAllSwap([this, &BoundKey] (const FInputAxisKeyBinding& Binding)
			{
				return Binding.AxisKey == BoundKey;
			});
			AxisKeyBoundKeys.Remove(BoundKey);
		}
	}
	else
	{
		AxisNameCounts.RemoveSingle(InActionSettings.AxisName, 0);
		if (AxisNameCounts.Contains(InActionSettings.AxisName)) return;

		for (int32 Index = InputComponent->AxisBindings.Num() - 1; Index >= 0; --Index)
		{
			const FInputAxisBinding& Binding = InputComponent->AxisBindings[Index];
			if (Binding.AxisName == InActionSettings.AxisName)
			{
				InputComponent->AxisBindings.RemoveAt(Index);
				break;
			}
		}
	}
}

void UVSInputAxisBinderFeature::RefreshAxes(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (int32 Index = 0; Index < AxisSettings.Num(); ++Index)
	{
		const auto& Axis = AxisSettings[Index];
		if (Axis.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (!BoundAxisSettings.Contains(Index))
			{
				BindAxisSettings(Axis);
				BoundAxisSettings.Add(Index);
			}
		}
		if (Axis.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (BoundAxisSettings.Contains(Index))
			{
				UnbindAxisSettings(Axis);
				BoundAxisSettings.Remove(Index);
			}
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

	APlayerController* PlayerController = Cast<APlayerController>(UVSActorLibrary::GetControllerFromActor(GetOwnerActor()));
	if (PlayerController && PlayerController->IsLocalController())
	{
		FeatureInputComponent = NewObject<UEnhancedInputComponent>(PlayerController);
		if (FeatureInputComponent.IsValid())
		{
			FeatureInputComponent->Priority = InputPriority;
			FeatureInputComponent->bBlockInput = bBlockInput;
			FeatureInputComponent->RegisterComponent();
			PlayerController->PushInputComponent(FeatureInputComponent.Get());

			GameplayTagFeaturePrivate = GetPrimaryGameplayTagFeature_Native();
			if (GameplayTagFeaturePrivate.IsValid())
			{
				GameplayTagFeaturePrivate->BindDelegateForObject(this);
			}
		}
	}
}

void UVSEnhancedInputActionBinderFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (FeatureInputComponent.IsValid())
	{
		/** Bind default actions. */
		for (int32 Index = 0; Index < ActionSettings.Num(); ++Index)
		{
			const auto& Action = ActionSettings[Index];
			if (Action.bBindByDefault && !BoundEnhancedActionSettings.Contains(Index))
			{
				BindActionSettings(Action);
				BoundEnhancedActionSettings.Add(Index);
			}
		}

		RefreshActions(FGameplayTagContainer::EmptyContainer);
	}
}

void UVSEnhancedInputActionBinderFeature::Uninitialize_Implementation()
{
	/** Unbind all actions. */
	if (FeatureInputComponent.IsValid())
	{
		for (int32 Index : BoundEnhancedActionSettings)
		{
			if (ActionSettings.IsValidIndex(Index))
			{
				UnbindActionSettings(ActionSettings[Index]);
			}
		}
		
		if (GameplayTagFeaturePrivate.IsValid())
		{
			GameplayTagFeaturePrivate->UnbindDelegateForObject(this);
		}

		if (APlayerController* PlayerController = Cast<APlayerController>(FeatureInputComponent->GetOwner()))
		{
			PlayerController->PopInputComponent(FeatureInputComponent.Get());
		}
		FeatureInputComponent->DestroyComponent();

		GameplayTagFeaturePrivate.Reset();
		BoundEnhancedActionSettings.Reset();
		FeatureInputComponent.Reset();
		EnhancedBindingHandlesByKey.Empty();
		EnhancedActionNameCounts.Empty();
	}
	
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
	return FeatureInputComponent.IsValid() ? FeatureInputComponent.Get() : nullptr;
}

void UVSEnhancedInputActionBinderFeature::BindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings)
{
	if (!FeatureInputComponent.IsValid() || !InActionSettings.IsValid()) return;

	for (const ETriggerEvent TriggerEvent : InActionSettings.TriggerEvents)
	{
		if (TriggerEvent == ETriggerEvent::None) continue;
		const uint64 BindingKey = MakeEnhancedBindingKey(InActionSettings.InputAction, TriggerEvent);
		EnhancedActionNameCounts.Add(BindingKey, InActionSettings.ActionName);

		if (const uint32* ExistingHandle = EnhancedBindingHandlesByKey.Find(BindingKey))
		{
			if (HasEnhancedBindingHandle(FeatureInputComponent.Get(), *ExistingHandle)) continue;
			EnhancedBindingHandlesByKey.Remove(BindingKey);
		}
		
		FEnhancedInputActionEventBinding& NewBinding = FeatureInputComponent->BindActionInstanceLambda(InActionSettings.InputAction, TriggerEvent,
			[this, BindingKey, TriggerEvent] (const FInputActionInstance& ActionInstance)
		{
			TArray<FName> ActionNames;
			EnhancedActionNameCounts.MultiFind(BindingKey, ActionNames);
			for (const FName& ActionName : ActionNames)
			{
				HandleActionExecution(ActionName, TriggerEvent, ActionInstance);
			}
		});
		EnhancedBindingHandlesByKey.Add(BindingKey, NewBinding.GetHandle());
	}
}

void UVSEnhancedInputActionBinderFeature::UnbindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings)
{
	if (!FeatureInputComponent.IsValid() || !InActionSettings.IsValid()) return;

	for (const ETriggerEvent TriggerEvent : InActionSettings.TriggerEvents)
	{
		if (TriggerEvent == ETriggerEvent::None) continue;

		const uint64 BindingKey = MakeEnhancedBindingKey(InActionSettings.InputAction, TriggerEvent);
		EnhancedActionNameCounts.RemoveSingle(BindingKey, InActionSettings.ActionName);
		if (EnhancedActionNameCounts.Contains(BindingKey)) continue;

		if (const uint32* ExistingHandle = EnhancedBindingHandlesByKey.Find(BindingKey))
		{
			if (HasEnhancedBindingHandle(FeatureInputComponent.Get(), *ExistingHandle))
			{
				FeatureInputComponent->RemoveBindingByHandle(*ExistingHandle);
			}
			EnhancedBindingHandlesByKey.Remove(BindingKey);
		}
	}
}

void UVSEnhancedInputActionBinderFeature::RefreshActions(const FGameplayTagContainer& TagEvents)
{
	if (!GameplayTagFeaturePrivate.IsValid()) return;
	const FGameplayTagContainer& GameplayTags = GameplayTagFeaturePrivate->GetGameplayTags();
	
	for (int32 Index = 0; Index < ActionSettings.Num(); ++Index)
	{
		const auto& Action = ActionSettings[Index];
		if (Action.AutoBindTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (!BoundEnhancedActionSettings.Contains(Index))
			{
				BindActionSettings(Action);
				BoundEnhancedActionSettings.Add(Index);
			}
		}
		if (Action.AutoRemoveTagQuery.Matches(TagEvents, GameplayTags))
		{
			if (BoundEnhancedActionSettings.Contains(Index))
			{
				UnbindActionSettings(Action);
				BoundEnhancedActionSettings.Remove(Index);
			}
		}
	}
}

void UVSEnhancedInputActionBinderFeature::HandleActionExecution(FName ActionName, ETriggerEvent TriggerEvent, const FInputActionInstance& ActionInstance)
{
	OnActionTriggered_Native.Broadcast(this, ActionName, TriggerEvent, ActionInstance);
	OnActionTriggered.Broadcast(this, ActionName, TriggerEvent, ActionInstance);
}
