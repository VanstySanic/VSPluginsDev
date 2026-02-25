// Copyright VanstySanic. All Rights Reserved.

#include "Items/Input/VSInputActionMappingKeySettingItem.h"
#include "Classes/Libraries/VSGameplayLibrary.h"
#include "Classes/Libraries/VSObjectLibrary.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/LocalPlayer.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSContainer.h"

UVSInputActionMappingKeySettingItem::UVSInputActionMappingKeySettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConfigSettings.FileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigSettings.Section = FString("InputMappingKey");

#if WITH_EDITOR
	EditorChangeActions = { EVSSettingItemAction::Confirm };
#endif
}

#if WITH_EDITOR
void UVSInputActionMappingKeySettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputActionMappingKeySettingItem, ActionName))
	{
		EditorPreviewDefaultChords = GetKeys(EVSSettingItemValueSource::Default);
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputActionMappingKeySettingItem, EditorPreviewSlottedChords))
	{
		SetKeys(EditorPreviewSlottedChords);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSInputActionMappingKeySettingItem::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	EditorPreviewDefaultChords = GetKeys(EVSSettingItemValueSource::Default);
#endif
}

void UVSInputActionMappingKeySettingItem::Load_Implementation()
{
	Super::Load_Implementation();

	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		FString ConfigString;
		if (GConfig && GConfig->GetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, ConfigString, ConfigSettings.FileName))
		{
			UVSObjectLibrary::ImportPropertyFromText(this, GET_MEMBER_NAME_CHECKED(UVSInputActionMappingKeySettingItem, CurrentSlottedChords), ConfigString);
		}
	}
}

void UVSInputActionMappingKeySettingItem::Save_Implementation()
{
	Super::Save_Implementation();

	if (!FName(ConfigSettings.PrimaryKey).IsNone())
	{
		const FString ConfigString = UVSObjectLibrary::ExportPropertyToText(this, GET_MEMBER_NAME_CHECKED(UVSInputActionMappingKeySettingItem, CurrentSlottedChords));
		GConfig->SetString(*ConfigSettings.Section, *ConfigSettings.PrimaryKey, *ConfigString, ConfigSettings.FileName);
	}
}

void UVSInputActionMappingKeySettingItem::Apply_Implementation()
{
	Super::Apply_Implementation();

	if (!ActionName.IsNone())
	{
		if (UPlayerInput* PlayerInput = GetPlayerInput())
		{
			for (const TPair<FVSInputMappingKeySlot, FInputChord>& SlottedChord : AppliedSlottedChords)
			{
				PlayerInput->RemoveActionMapping(MakeActionMapping(ActionName, SlottedChord.Value));
			}

			for (const TPair<FVSInputMappingKeySlot, FInputChord>& SlottedChord : CurrentSlottedChords)
			{
				PlayerInput->AddActionMapping(MakeActionMapping(ActionName, SlottedChord.Value));
			}
		
			PlayerInput->ForceRebuildingKeyMaps(false);
		}
	}
	
	AppliedSlottedChords = CurrentSlottedChords;
}

void UVSInputActionMappingKeySettingItem::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	ConfirmedSlottedChords = CurrentSlottedChords;
}

void UVSInputActionMappingKeySettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
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

bool UVSInputActionMappingKeySettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return FVSMap::MapEqual(GetKeys(EVSSettingItemValueSource::System), GetKeys(ValueSource));
}

TMap<FVSInputMappingKeySlot, FInputChord> UVSInputActionMappingKeySettingItem::GetKeys(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return GetChordsFromInputSettings();
		
	case EVSSettingItemValueSource::Game:
		if (GetPlayerInput())
		{
			return GetChordsFromPlayerInput();
		}
		return GetKeys(EVSSettingItemValueSource::System);
		
	case EVSSettingItemValueSource::System:
		return CurrentSlottedChords;
		
	case EVSSettingItemValueSource::Confirmed:
		return ConfirmedSlottedChords;
		
	default: ;
	}

	return TMap<FVSInputMappingKeySlot, FInputChord>();
}

FInputChord UVSInputActionMappingKeySettingItem::GetKey(const FVSInputMappingKeySlot& Slot, EVSSettingItemValueSource::Type ValueSource) const
{
	return GetKeys(ValueSource).FindRef(Slot);
}

void UVSInputActionMappingKeySettingItem::SetKeys(const TMap<FVSInputMappingKeySlot, FInputChord>& SlottedChords)
{
	const TMap<FVSInputMappingKeySlot, FInputChord> PrevSlottedChords = CurrentSlottedChords;
	
	CurrentSlottedChords = SlottedChords;
	for (const TPair<FVSInputMappingKeySlot, FInputChord> SlottedChord : SlottedChords)
	{
		if (!SlottedChord.Value.Key.IsValid() &&SlottedChord.Value == GetKey(SlottedChord.Key, EVSSettingItemValueSource::Default))
		{
			CurrentSlottedChords.Remove(SlottedChord.Key);
		}
	}

	if (!FVSMap::MapEqual(PrevSlottedChords, CurrentSlottedChords))
	{
		NotifyValueUpdated(true);
	}
}

void UVSInputActionMappingKeySettingItem::SetKey(const FVSInputMappingKeySlot& Slot, const FInputChord& Chord)
{
	const FInputChord PrevChord = CurrentSlottedChords.FindRef(Slot);
	
	CurrentSlottedChords.Emplace(Slot, Chord);
	if (!Chord.Key.IsValid() && Chord == GetKey(Slot, EVSSettingItemValueSource::Default))
	{
		CurrentSlottedChords.Remove(Slot);
	}

	if (PrevChord != Chord)
	{
		NotifyValueUpdated(true);
	}
}

FInputActionKeyMapping UVSInputActionMappingKeySettingItem::MakeActionMapping(FName InActionName, const FInputChord& Chord)
{
	FInputActionKeyMapping Mapping;
	Mapping.ActionName = InActionName;
	Mapping.Key = Chord.Key;
	Mapping.bShift = Chord.bShift;
	Mapping.bCtrl = Chord.bCtrl;
	Mapping.bAlt = Chord.bAlt;
	Mapping.bCmd = Chord.bCmd;
	return Mapping;
}

FInputChord UVSInputActionMappingKeySettingItem::MakeChord(const FInputActionKeyMapping& Mapping)
{
	FInputChord Chord;
	Chord.Key = Mapping.Key;
	Chord.bShift = Mapping.bShift;
	Chord.bCtrl = Mapping.bCtrl;
	Chord.bAlt = Mapping.bAlt;
	Chord.bCmd = Mapping.bCmd;
	return Chord;
}

UPlayerInput* UVSInputActionMappingKeySettingItem::GetPlayerInput() const
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

TMap<FVSInputMappingKeySlot, FInputChord> UVSInputActionMappingKeySettingItem::GetChordsFromPlayerInput() const
{
	const UPlayerInput* PlayerInput = GetPlayerInput();
	if (!PlayerInput || ActionName.IsNone())
	{
		return TMap<FVSInputMappingKeySlot, FInputChord>();
	}

	TMap<FVSInputMappingKeySlot, FInputChord> SlottedChords;
	int32 SlotIndex = 0;
	for (const FInputActionKeyMapping& Mapping : PlayerInput->ActionMappings)
	{
		if (Mapping.ActionName == ActionName)
		{
			SlottedChords.Emplace(FVSInputMappingKeySlot(SlotIndex), MakeChord(Mapping));
			SlotIndex++;
		}
	}

	return SlottedChords;
}

TMap<FVSInputMappingKeySlot, FInputChord> UVSInputActionMappingKeySettingItem::GetChordsFromInputSettings() const
{
	if (!UInputSettings::GetInputSettings() || ActionName.IsNone())
	{
		return TMap<FVSInputMappingKeySlot, FInputChord>();
	}

	const TArray<FInputActionKeyMapping> ActionMappings = UInputSettings::GetInputSettings()->GetActionMappings();

	TMap<FVSInputMappingKeySlot, FInputChord> SlottedChords;
	int32 SlotIndex = 0;
	for (const FInputActionKeyMapping& Mapping : ActionMappings)
	{
		if (Mapping.ActionName == ActionName)
		{
			SlottedChords.Emplace(FVSInputMappingKeySlot(SlotIndex), MakeChord(Mapping));
			SlotIndex++;
		}
	}

	return SlottedChords;
}
