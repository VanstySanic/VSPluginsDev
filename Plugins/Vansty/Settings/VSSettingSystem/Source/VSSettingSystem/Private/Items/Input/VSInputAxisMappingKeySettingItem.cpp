// Copyright VanstySanic. All Rights Reserved.

#include "Items/Input/VSInputAxisMappingKeySettingItem.h"
#include "Classes/Libraries/VSGameplayLibrary.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/LocalPlayer.h"
#include "Types/Math/VSContainer.h"

UVSInputAxisMappingKeySettingItem::UVSInputAxisMappingKeySettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConfigSettings.FileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigSettings.Section = FString("InputMappingAxis");

#if WITH_EDITOR
	EditorChangeActions = { EVSSettingItemAction::Confirm };
#endif
}

#if WITH_EDITOR
void UVSInputAxisMappingKeySettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputAxisMappingKeySettingItem, AxisName))
	{
		EditorPreviewDefaultKeys = GetKeys(EVSSettingItemValueSource::Default);
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputAxisMappingKeySettingItem, EditorPreviewSlottedKeys))
	{
		SetKeys(EditorPreviewSlottedKeys);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSInputAxisMappingKeySettingItem::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	EditorPreviewDefaultKeys = GetKeys(EVSSettingItemValueSource::Default);
#endif
}

void UVSInputAxisMappingKeySettingItem::Load_Implementation()
{
	Super::Load_Implementation();

	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		FString ConfigString;
		if (GConfig && GConfig->GetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, ConfigString, ConfigSettings.FileName))
		{
			UVSObjectLibrary::ImportPropertyFromText(this, GET_MEMBER_NAME_CHECKED(UVSInputAxisMappingKeySettingItem, CurrentSlottedKeys), ConfigString);
		}
	}
}

void UVSInputAxisMappingKeySettingItem::Save_Implementation()
{
	Super::Save_Implementation();

	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		const FString ConfigString = UVSObjectLibrary::ExportPropertyToText(this, GET_MEMBER_NAME_CHECKED(UVSInputAxisMappingKeySettingItem, CurrentSlottedKeys));
		GConfig->SetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, *ConfigString, ConfigSettings.FileName);
	}
}

void UVSInputAxisMappingKeySettingItem::Apply_Implementation()
{
	Super::Apply_Implementation();
	
	if (!AxisName.IsNone())
	{
		if (UPlayerInput* PlayerInput = GetPlayerInput())
        {
        	{
        		TArray<int32> RemovalIndices;
        		for (const TPair<FVSInputMappingAxisSlot, FKey>& SlottedKey : AppliedSlottedKeys)
        		{
        			if (!SlottedKey.Value.IsValid())
        			{
        				continue;
        			}
    
        			const int32 TargetSlotIndex = SlottedKey.Key.Index;
        			int32 CurrentSlot = 0;
        			for (int32 Index = 0; Index < PlayerInput->AxisMappings.Num(); ++Index)
        			{
        				const FInputAxisKeyMapping& Mapping = PlayerInput->AxisMappings[Index];
        				if (Mapping.AxisName != AxisName)
        				{
        					continue;
        				}
    
        				if (CurrentSlot == TargetSlotIndex && Mapping.Key == SlottedKey.Value)
        				{
        					RemovalIndices.Add(Index);
        					break;
        				}
        				CurrentSlot++;
        			}
        		}
    
        		RemovalIndices.Sort(TGreater<int32>());
        		for (const int32 AxisIndex : RemovalIndices)
        		{
        			PlayerInput->AxisMappings.RemoveAt(AxisIndex);
        		}
    
        		TArray<int32> AxisMappingIndices;
        		for (int32 Index = 0; Index < PlayerInput->AxisMappings.Num(); ++Index)
        		{
        			if (PlayerInput->AxisMappings[Index].AxisName == AxisName)
        			{
        				AxisMappingIndices.Add(Index);
        			}
        		}
    
        		TMap<int32, FKey> DesiredKeysBySlot;
        		for (const TPair<FVSInputMappingAxisSlot, FKey>& SlottedKey : CurrentSlottedKeys)
        		{
        			DesiredKeysBySlot.Add(SlottedKey.Key.Index, SlottedKey.Value);
        		}
    
        		TArray<int32> RemovalIndicesForEmpty;
        		for (const TPair<int32, FKey>& DesiredPair : DesiredKeysBySlot)
        		{
        			const int32 SlotIndex = DesiredPair.Key;
        			const FKey DesiredKey = DesiredPair.Value;
    
        			if (!DesiredKey.IsValid())
        			{
        				if (AxisMappingIndices.IsValidIndex(SlotIndex))
        				{
        					RemovalIndicesForEmpty.Add(AxisMappingIndices[SlotIndex]);
        				}
        				continue;
        			}
    
        			if (AxisMappingIndices.IsValidIndex(SlotIndex))
        			{
        				PlayerInput->AxisMappings[AxisMappingIndices[SlotIndex]].Key = DesiredKey;
        			}
        			else
        			{
        				PlayerInput->AddAxisMapping(MakeAxisMapping(DesiredKey, GetDesiredScaleForSlot(SlotIndex, PlayerInput)));
        			}
        		}
    
        		RemovalIndicesForEmpty.Sort(TGreater<int32>());
        		for (const int32 AxisIndex : RemovalIndicesForEmpty)
        		{
        			PlayerInput->AxisMappings.RemoveAt(AxisIndex);
        		}
    
        		PlayerInput->ForceRebuildingKeyMaps(false);
        	}
        }
	}
	
	AppliedSlottedKeys = CurrentSlottedKeys;
}

void UVSInputAxisMappingKeySettingItem::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	ConfirmedSlottedKeys = CurrentSlottedKeys;
}

void UVSInputAxisMappingKeySettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	if (ValueSource == EVSSettingItemValueSource::Default)
	{
		SetKeys({});
	}
	else
	{
		SetKeys(GetKeys(ValueSource));
	}
}

bool UVSInputAxisMappingKeySettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return FVSMap::MapEqual(GetKeys(EVSSettingItemValueSource::System), GetKeys(ValueSource));
}

TMap<FVSInputMappingAxisSlot, FKey> UVSInputAxisMappingKeySettingItem::GetKeys(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return GetKeysFromInputSettings(GetDefault<UInputSettings>());
		
	case EVSSettingItemValueSource::Game:
		if (const UPlayerInput* PlayerInput = GetPlayerInput())
		{
			return GetKeysFromPlayerInput(PlayerInput);
		}
		
		return GetKeys(EVSSettingItemValueSource::System);
		
	case EVSSettingItemValueSource::System:
		return CurrentSlottedKeys;
		
	case EVSSettingItemValueSource::Confirmed:
		return ConfirmedSlottedKeys;
		
	default: ;
	}

	return TMap<FVSInputMappingAxisSlot, FKey>();
}

void UVSInputAxisMappingKeySettingItem::SetKeys(const TMap<FVSInputMappingAxisSlot, FKey>& SlottedKeys)
{
	const TMap<FVSInputMappingAxisSlot, FKey> PrevSlottedMappings = CurrentSlottedKeys;
	
	CurrentSlottedKeys = SlottedKeys;
	for (const TPair<FVSInputMappingAxisSlot, FKey> SlottedKey : SlottedKeys)
	{
		if (!SlottedKey.Value.IsValid())
		{
			CurrentSlottedKeys.Remove(SlottedKey.Key);
		}
	}

	if (!FVSMap::MapEqual(PrevSlottedMappings, CurrentSlottedKeys))
	{
		NotifyValueUpdated(true);
	}
}

FKey UVSInputAxisMappingKeySettingItem::GetKey(const FVSInputMappingAxisSlot& Slot, EVSSettingItemValueSource::Type ValueSource) const
{
	return GetKeys(ValueSource).FindRef(Slot);
}

void UVSInputAxisMappingKeySettingItem::SetKey(const FVSInputMappingAxisSlot& Slot, const FKey& Key)
{
	const FKey PrevKey = CurrentSlottedKeys.FindRef(Slot);
	CurrentSlottedKeys.Emplace(Slot, Key);

	if (!Key.IsValid())
	{
		CurrentSlottedKeys.Remove(Slot);
	}

	if (PrevKey != Key)
	{
		NotifyValueUpdated(true);
	}
}

FInputAxisKeyMapping UVSInputAxisMappingKeySettingItem::MakeAxisMapping(const FKey& Key, float Scale) const
{
	return FInputAxisKeyMapping(AxisName, Key, Scale);
}

float UVSInputAxisMappingKeySettingItem::GetDesiredScaleForSlot(int32 SlotIndex, const UPlayerInput* PlayerInput) const
{
	if (PlayerInput)
	{
		int32 CurrentSlot = 0;
		for (const FInputAxisKeyMapping& Mapping : PlayerInput->AxisMappings)
		{
			if (Mapping.AxisName == AxisName)
			{
				if (CurrentSlot == SlotIndex)
				{
					return Mapping.Scale;
				}
				++CurrentSlot;
			}
		}
	}

	if (const UInputSettings* InputSettings = GetDefault<UInputSettings>())
	{
		int32 CurrentSlot = 0;
		for (const FInputAxisKeyMapping& Mapping : InputSettings->GetAxisMappings())
		{
			if (Mapping.AxisName == AxisName)
			{
				if (CurrentSlot == SlotIndex)
				{
					return Mapping.Scale;
				}
				++CurrentSlot;
			}
		}
	}

	return 1.0f;
}

UPlayerInput* UVSInputAxisMappingKeySettingItem::GetPlayerInput() const
{
	if (UWorld* World = UVSGameplayLibrary::GetPossibleGameplayWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			const int32 LocalPlayerIndexToUse = LocalPlayerIndex < 0 ? 0 : LocalPlayerIndex;
			if (ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(LocalPlayerIndexToUse))
			{
				if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(World))
				{
					return PlayerController->PlayerInput;
				}
			}
		}
	}

	return nullptr;
}

TMap<FVSInputMappingAxisSlot, FKey> UVSInputAxisMappingKeySettingItem::GetKeysFromPlayerInput(const UPlayerInput* PlayerInput) const
{
	if (!PlayerInput || AxisName.IsNone())
	{
		return TMap<FVSInputMappingAxisSlot, FKey>();
	}

	TMap<FVSInputMappingAxisSlot, FKey> SlottedKeys;
	int32 SlotIndex = 0;
	for (const FInputAxisKeyMapping& Mapping : PlayerInput->AxisMappings)
	{
		if (Mapping.AxisName == AxisName)
		{
			SlottedKeys.Emplace(FVSInputMappingAxisSlot(SlotIndex), Mapping.Key);
			SlotIndex++;
		}
	}

	return SlottedKeys;
}

TMap<FVSInputMappingAxisSlot, FKey> UVSInputAxisMappingKeySettingItem::GetKeysFromInputSettings(const UInputSettings* InputSettings) const
{
	if (!InputSettings || AxisName.IsNone())
	{
		return TMap<FVSInputMappingAxisSlot, FKey>();
	}

	const TArray<FInputAxisKeyMapping> AxisMappings = InputSettings->GetAxisMappings();

	TMap<FVSInputMappingAxisSlot, FKey> SlottedKeys;
	int32 SlotIndex = 0;
	for (const FInputAxisKeyMapping& Mapping : AxisMappings)
	{
		if (Mapping.AxisName == AxisName)
		{
			SlottedKeys.Emplace(FVSInputMappingAxisSlot(SlotIndex), Mapping.Key);
			SlotIndex++;
		}
	}

	return SlottedKeys;
}
