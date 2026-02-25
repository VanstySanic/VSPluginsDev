// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSSettingItemInputKeyWidgetBinder.h"
#include "VSPrivablic.h"
#include "Components/InputKeySelector.h"
#include "Components/VSContentedInputKeySelector.h"
#include "Components/VSInputKeySelectorGroupWidget.h"
#include "Items/Input/VSEnhancedInputMappingKeySettingItem.h"
#include "Items/Input/VSInputActionMappingKeySettingItem.h"
#include "Items/Input/VSInputAxisMappingKeySettingItem.h"
#include "Mediator/VSWidgetController.h"

VS_DECLARE_PRIVABLIC_MEMBER(UVSEnhancedInputMappingKeySettingItem, HardwareDeviceID, FName);

UVSSettingItemInputKeyWidgetBinder::UVSSettingItemInputKeyWidgetBinder(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	
}

void UVSSettingItemInputKeyWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Keys"))
	{
		if (UVSInputKeySelectorGroupWidget* KeyGroup = Cast<UVSInputKeySelectorGroupWidget>(Widget))
		{
			KeyGroup->KeySettings = KeySettings;
		}
	}
	
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UVSInputKeySelectorGroupWidget* KeyGroup = Cast<UVSInputKeySelectorGroupWidget>(Widget))
		{
			GetWidgetController()->BindWidget(KeyGroup, FName("Keys"));
		}
		else if (UVSContentedInputKeySelector* ContentedKey = Cast<UVSContentedInputKeySelector>(Widget))
		{
			GetWidgetController()->BindWidget(ContentedKey, FName("Keys"));
		}
		else if (UInputKeySelector* KeySelector = Cast<UInputKeySelector>(Widget))
		{
			GetWidgetController()->BindWidget(KeySelector, FName("Keys"));
		}
	}
}

void UVSSettingItemInputKeyWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Core") && GetWidgetController())
	{
		if (UVSInputKeySelectorGroupWidget* KeyGroup = Cast<UVSInputKeySelectorGroupWidget>(Widget))
		{
			GetWidgetController()->UnbindWidget(KeyGroup, FName("Keys"));
		}
		else if (UVSContentedInputKeySelector* ContentedKey = Cast<UVSContentedInputKeySelector>(Widget))
		{
			GetWidgetController()->UnbindWidget(ContentedKey, FName("Keys"));
		}
		else if (UInputKeySelector* KeySelector = Cast<UInputKeySelector>(Widget))
		{
			GetWidgetController()->UnbindWidget(KeySelector, FName("Keys"));
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSSettingItemInputKeyWidgetBinder::OnWidgetKeysUpdated_Implementation(const TArray<FInputChord>& NewKeys)
{
	Super::OnWidgetKeysUpdated_Implementation(NewKeys);

	if (UVSInputActionMappingKeySettingItem* ActionKeyItem = Cast<UVSInputActionMappingKeySettingItem>(GetSettingItem_Native()))
	{
		TMap<FVSInputMappingKeySlot, FInputChord> ActionKeyMap;
		for (int32 Index = 0; Index < KeySlots.Num(); Index++)
		{
			if (NewKeys.IsValidIndex(Index))
			{
				ActionKeyMap.Add(FVSInputMappingKeySlot(KeySlots[Index]), NewKeys[Index]);
			}
		}
		ActionKeyItem->SetKeys(ActionKeyMap);
	}
	else if (UVSInputAxisMappingKeySettingItem* AxisKeyItem = Cast<UVSInputAxisMappingKeySettingItem>(GetSettingItem_Native()))
	{
		TMap<FVSInputMappingAxisSlot, FKey> AxisKeyMap;
		for (int32 Index = 0; Index < KeySlots.Num(); Index++)
		{
			if (NewKeys.IsValidIndex(Index))
			{
				AxisKeyMap.Add(FVSInputMappingAxisSlot(KeySlots[Index]), NewKeys[Index].Key);
			}
		}
		AxisKeyItem->SetKeys(AxisKeyMap);
	}
	else if (UVSEnhancedInputMappingKeySettingItem* EnhancedKeyItem = Cast<UVSEnhancedInputMappingKeySettingItem>(GetSettingItem_Native()))
	{
		TMap<FVSEnhancedInputMappingKeySlot, FKey> EnhancedKeyMap;
		for (int32 Index = 0; Index < KeySlots.Num(); Index++)
		{
			if (NewKeys.IsValidIndex(Index))
			{
				EnhancedKeyMap.Add(FVSEnhancedInputMappingKeySlot(KeySlots[Index]), NewKeys[Index].Key);
			}
		}
		EnhancedKeyItem->SetKeys(EnhancedKeyMap);
	}
}

void UVSSettingItemInputKeyWidgetBinder::OnCurrentSettingItemUpdated_Implementation()
{
	RebindWidgetByType(FName("Keys"));
}

TArray<FInputChord> UVSSettingItemInputKeyWidgetBinder::GetExternalKeys_Implementation() const
{
	TArray<FInputChord> OutKeys;
	
	if (UVSInputActionMappingKeySettingItem* ActionKeyItem = Cast<UVSInputActionMappingKeySettingItem>(GetSettingItem_Native()))
	{
		const auto SystemKeys = ActionKeyItem->GetKeys(EVSSettingItemValueSource::System);
		const auto GameKeys = ActionKeyItem->GetKeys(EVSSettingItemValueSource::Game);
		const auto DefaultKeys = ActionKeyItem->GetKeys(EVSSettingItemValueSource::Default);
		for (int32 KeySlot : KeySlots)
		{
			if (SystemKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(SystemKeys[KeySlot]);
			}
			else if (GameKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(GameKeys[KeySlot]);
			}
			else if (DefaultKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(DefaultKeys[KeySlot]);
			}
		}
	}
	else if (UVSInputAxisMappingKeySettingItem* AxisKeyItem = Cast<UVSInputAxisMappingKeySettingItem>(GetSettingItem_Native()))
	{
		const auto SystemKeys = AxisKeyItem->GetKeys(EVSSettingItemValueSource::System);
		const auto GameKeys = AxisKeyItem->GetKeys(EVSSettingItemValueSource::Game);
		const auto DefaultKeys = AxisKeyItem->GetKeys(EVSSettingItemValueSource::Default);
		for (int32 KeySlot : KeySlots)
		{
			if (SystemKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(SystemKeys[KeySlot]);
			}
			else if (GameKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(GameKeys[KeySlot]);
			}
			else if (DefaultKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(DefaultKeys[KeySlot]);
			}
		}
	}
	else if (UVSEnhancedInputMappingKeySettingItem* EnhancedKeyItem = Cast<UVSEnhancedInputMappingKeySettingItem>(GetSettingItem_Native()))
	{
		const auto SystemKeys = EnhancedKeyItem->GetKeys(EVSSettingItemValueSource::System);
		const auto GameKeys = EnhancedKeyItem->GetKeys(EVSSettingItemValueSource::Game);
		const auto DefaultKeys = EnhancedKeyItem->GetKeys(EVSSettingItemValueSource::Default);
		for (int32 KeySlot : KeySlots)
		{
			if (SystemKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(SystemKeys[KeySlot]);
			}
			else if (GameKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(GameKeys[KeySlot]);
			}
			else if (DefaultKeys.Contains(KeySlot))
			{
				OutKeys.Emplace(DefaultKeys[KeySlot]);
			}
		}
	}
	
	return OutKeys;
}

#if WITH_EDITOR
void UVSSettingItemInputKeyWidgetBinder::RefreshKeySettings()
{
	if (UVSSettingItemBase* SettingItem = GetSettingItem_Native())
	{
		if (Cast<UVSInputActionMappingKeySettingItem>(SettingItem))
		{
			KeySettings.bOverrideAllowModifierKeys = true;
			KeySettings.bAllowModifierKeys = true;
		}
		else if (Cast<UVSInputAxisMappingKeySettingItem>(SettingItem))
		{
			KeySettings.bOverrideAllowModifierKeys = true;
			KeySettings.bAllowModifierKeys = false;
		}
		else if (UVSEnhancedInputMappingKeySettingItem* EnhancedKeyItem = Cast<UVSEnhancedInputMappingKeySettingItem>(SettingItem))
		{
			KeySettings.bOverrideAllowModifierKeys = true;
			KeySettings.bAllowModifierKeys = false;

			const FName HardwareDeviceID = VS_PRIVABLIC_MEMBER(EnhancedKeyItem, UVSEnhancedInputMappingKeySettingItem, HardwareDeviceID);
			if (HardwareDeviceID == FName("KBM"))
			{
				KeySettings.bOverrideAllowGamepadKeys = true;
				KeySettings.bOverrideAllowKeyboardKeys = true;
				KeySettings.bOverrideAllowMouseKeys = true;
				KeySettings.bAllowGamepadKeys = false;
				KeySettings.bAllowMouseKeys = true;
				KeySettings.bAllowKeyboardKeys = true;
			}
			else if (HardwareDeviceID == FName("MobileTouch"))
			{
				KeySettings.bOverrideAllowGamepadKeys = true;
				KeySettings.bOverrideAllowKeyboardKeys = true;
				KeySettings.bOverrideAllowMouseKeys = true;
				KeySettings.bAllowGamepadKeys = false;
				KeySettings.bAllowMouseKeys = false;
				KeySettings.bAllowKeyboardKeys = false;
			}
			else // if (HardwareDeviceID == FName("Gamepad"))
			{
				KeySettings.bOverrideAllowGamepadKeys = true;
				KeySettings.bOverrideAllowKeyboardKeys = true;
				KeySettings.bOverrideAllowMouseKeys = true;
				KeySettings.bAllowGamepadKeys = true;
				KeySettings.bAllowMouseKeys = false;
				KeySettings.bAllowKeyboardKeys = false;
			}
		}
	}

	if (UWidget* KeysWidget = GetBoundTypedWidget(FName("Keys")))
	{
		if (UVSInputKeySelectorGroupWidget* KeyGroup = Cast<UVSInputKeySelectorGroupWidget>(KeysWidget))
		{
			KeyGroup->KeySettings = KeySettings;
			KeyGroup->RefreshKeySelectors();
		}
		else if (UVSContentedInputKeySelector* ContentedKey = Cast<UVSContentedInputKeySelector>(KeysWidget))
		{
			KeySettings.ApplyToKeySelector(ContentedKey);
		}
		else if (UInputKeySelector* KeySelector = Cast<UInputKeySelector>(KeysWidget))
		{
			KeySettings.ApplyToKeySelector(KeySelector);
		}
	}
}

void UVSSettingItemInputKeyWidgetBinder::EditorRefreshMediator_Implementation()
{
	RefreshKeySettings();
}
#endif
