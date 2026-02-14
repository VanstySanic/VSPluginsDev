// Copyright VanstySanic. All Rights Reserved.

#include "Items/Input/VSEnhancedInputMappingKeySettingItem.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagsManager.h"
#include "InputMappingContext.h"
#include "Classes/Libraries/VSGameplayLibrary.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "Types/Math/VSContainer.h"
#include "UObject/UnrealType.h"

UVSEnhancedInputMappingKeySettingItem::UVSEnhancedInputMappingKeySettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConfigSettings.FileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigSettings.Section = FString("EnhancedInputMappingKey");
	
	ProfileId = UGameplayTagsManager::Get().RequestGameplayTag("InputUserSettings.Profiles.Default");
	
#if WITH_EDITOR
	EditorChangeActions = { EVSSettingItemAction::Confirm };
#endif
}

#if WITH_EDITOR
void UVSEnhancedInputMappingKeySettingItem::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, ProfileId)
		||PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, MappingContext)
		|| PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, MappingName))
	{
		EditorPreviewDefaultKeys = GetKeys(EVSSettingItemValueSource::Default);
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, EditorPreviewSlottedKeys))
	{
		SetKeys(EditorPreviewSlottedKeys);
	}
	
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif

void UVSEnhancedInputMappingKeySettingItem::PostLoad()
{
	Super::PostLoad();
	
#if WITH_EDITOR
	EditorPreviewDefaultKeys = GetKeys(EVSSettingItemValueSource::Default);
#endif
}

void UVSEnhancedInputMappingKeySettingItem::Load_Implementation()
{
	Super::Load_Implementation();
	
	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		FString ConfigString;
		if (GConfig && GConfig->GetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, ConfigString, ConfigSettings.FileName))
		{
			UVSObjectLibrary::ImportPropertyFromText(this, GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, CurrentSlottedKeys), ConfigString);
		}
	}
}

void UVSEnhancedInputMappingKeySettingItem::Save_Implementation()
{
	Super::Save_Implementation();

	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		const FString ConfigString = UVSObjectLibrary::ExportPropertyToText(this, GET_MEMBER_NAME_CHECKED(UVSEnhancedInputMappingKeySettingItem, CurrentSlottedKeys));
		GConfig->SetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, *ConfigString, ConfigSettings.FileName);
	}
}

void UVSEnhancedInputMappingKeySettingItem::Apply_Implementation()
{
	Super::Apply_Implementation();

	if (UEnhancedInputUserSettings* EnhancedInputUserSettings = GetEnhancedInputUserSettings())
	{
		for (const TPair<FVSEnhancedInputMappingKeySlot, FKey> SlottedKey : AppliedSlottedKeys)
		{
			FGameplayTagContainer FailureReason;
			EnhancedInputUserSettings->UnMapPlayerKey(MakeMapPlayerKeyArgs(SlottedKey.Key, SlottedKey.Value), FailureReason);
		}

		for (const TPair<FVSEnhancedInputMappingKeySlot, FKey> SlottedKey : CurrentSlottedKeys)
		{
			FGameplayTagContainer FailureReason;
			EnhancedInputUserSettings->MapPlayerKey(MakeMapPlayerKeyArgs(SlottedKey.Key, SlottedKey.Value), FailureReason);
		}
	}

	AppliedSlottedKeys = CurrentSlottedKeys;
}

void UVSEnhancedInputMappingKeySettingItem::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	ConfirmedSlottedKeys = CurrentSlottedKeys;
}

void UVSEnhancedInputMappingKeySettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
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

bool UVSEnhancedInputMappingKeySettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return FVSMap::MapEqual(GetKeys(EVSSettingItemValueSource::System), GetKeys(ValueSource));
}

TMap<FVSEnhancedInputMappingKeySlot, FKey> UVSEnhancedInputMappingKeySettingItem::GetKeys(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		{
			TMap<FVSEnhancedInputMappingKeySlot, FKey> SlottedKeys = GetKeysFromUserSettings(true);
			if (SlottedKeys.IsEmpty())
			{
				SlottedKeys = GetKeysFromMappingContext();
			}
			return SlottedKeys;
		}
	
	case EVSSettingItemValueSource::Game:
		if (GetEnhancedInputUserSettings())
		{
			return GetKeysFromUserSettings(false);
		}
		return GetKeys(EVSSettingItemValueSource::System);
		
	case EVSSettingItemValueSource::System:
		return CurrentSlottedKeys;
		
	case EVSSettingItemValueSource::Confirmed:
		return ConfirmedSlottedKeys;
		
	default: ;
	}
	
	return TMap<FVSEnhancedInputMappingKeySlot, FKey>();
}

void UVSEnhancedInputMappingKeySettingItem::SetKeys(const TMap<FVSEnhancedInputMappingKeySlot, FKey>& SlottedKeys)
{
	const TMap<FVSEnhancedInputMappingKeySlot, FKey> PrevSlottedKeys = CurrentSlottedKeys;

	CurrentSlottedKeys = SlottedKeys;
	for (const TPair<FVSEnhancedInputMappingKeySlot, FKey>& SlottedKey : SlottedKeys)
	{
		if (!SlottedKey.Value.IsValid())
		{
			CurrentSlottedKeys.Remove(SlottedKey.Key);
		}
	}

	if (!FVSMap::MapEqual(PrevSlottedKeys, CurrentSlottedKeys))
	{
		NotifyValueUpdated(true);
	}
}

FKey UVSEnhancedInputMappingKeySettingItem::GetKey(const FVSEnhancedInputMappingKeySlot& Slot, EVSSettingItemValueSource::Type ValueSource) const
{
	return GetKeys(ValueSource).FindRef(Slot);
}

void UVSEnhancedInputMappingKeySettingItem::SetKey(const FVSEnhancedInputMappingKeySlot& Slot, const FKey& Key)
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

TArray<FName> UVSEnhancedInputMappingKeySettingItem::GetMappingNames() const
{
	if (!MappingContext) return TArray<FName>( { NAME_None } );
	TArray<FName> MappingNames = TArray<FName>( { NAME_None } );
	
	for (const FEnhancedActionKeyMapping Mapping : MappingContext->GetMappings())
	{
		if (!Mapping.GetMappingName().IsNone())
		{
			MappingNames.AddUnique(Mapping.GetMappingName()) ;
		}
	}

	return MappingNames;
}

UEnhancedInputUserSettings* UVSEnhancedInputMappingKeySettingItem::GetEnhancedInputUserSettings() const
{
	if (UWorld* World = UVSGameplayLibrary::GetPossibleGameplayWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			int32 LocalPlayerIndexToUse = LocalPlayerIndex < 0 ? 0 : LocalPlayerIndex;
			if (ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(LocalPlayerIndexToUse))
			{
				if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (UEnhancedInputUserSettings* EnhancedInputUserSettings = EnhancedInputLocalPlayerSubsystem->GetUserSettings())
					{
						return EnhancedInputUserSettings;
					}
				}
			}
		}
	}

	return nullptr;
}

TMap<FVSEnhancedInputMappingKeySlot, FKey> UVSEnhancedInputMappingKeySettingItem::GetKeysFromUserSettings(bool bUseDefault) const
{
	TMap<FVSEnhancedInputMappingKeySlot, FKey> SlottedKeys;
	if (UEnhancedInputUserSettings* EnhancedInputUserSettings = GetEnhancedInputUserSettings())
	{
		for (const FPlayerKeyMapping FindMappingsInRow : EnhancedInputUserSettings->FindMappingsInRow(MappingName))
		{
			const FVSEnhancedInputMappingKeySlot Slot(FindMappingsInRow.GetSlot(), FindMappingsInRow.GetHardwareDeviceId().HardwareDeviceIdentifier);
			SlottedKeys.Emplace(Slot, bUseDefault ? FindMappingsInRow.GetDefaultKey() : FindMappingsInRow.GetCurrentKey());
		}
	}

	return SlottedKeys;
}

TMap<FVSEnhancedInputMappingKeySlot, FKey> UVSEnhancedInputMappingKeySettingItem::GetKeysFromMappingContext() const
{
	TMap<FVSEnhancedInputMappingKeySlot, FKey> SlottedKeys;
	if (!MappingContext) return SlottedKeys;
	
	TArray<FKey> DefaultKeys;
	for (const FEnhancedActionKeyMapping Mapping : MappingContext->GetMappings())
	{
		if (!Mapping.GetMappingName().IsNone() && Mapping.GetMappingName() == MappingName)
		{
			DefaultKeys.Add(Mapping.Key);
		}
	}
	
	for (int32 i = 0; i < int32(EPlayerMappableKeySlot::Max); ++i)
	{
		if (!DefaultKeys.IsValidIndex(i)) break;
		SlottedKeys.Emplace(EPlayerMappableKeySlot(i), DefaultKeys[i]);
	}

	return SlottedKeys;
}

FMapPlayerKeyArgs UVSEnhancedInputMappingKeySettingItem::MakeMapPlayerKeyArgs(FVSEnhancedInputMappingKeySlot Slot, FKey Key) const
{
	FMapPlayerKeyArgs Args;
	
	Args.MappingName = MappingName;
	Args.ProfileId = ProfileId;
	Args.Slot = Slot.Slot;
	Args.HardwareDeviceId = Slot.HardwareDeviceID;
	Args.NewKey = Key;
	
	Args.bCreateMatchingSlotIfNeeded = true;
	Args.bDeferOnSettingsChangedBroadcast = true;

	return Args;
}
