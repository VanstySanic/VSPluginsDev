// Copyright VanstySanic. All Rights Reserved.

#include "Items/VSMutableFloatSettingItem.h"
#include "Kismet/KismetTextLibrary.h"

UVSMutableFloatSettingItem::UVSMutableFloatSettingItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR
void UVSMutableFloatSettingItem::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSMutableFloatSettingItem, ConfigParams))
	{
		/** Clear previous config and save new one. */
		float ConfigValue;
		bool bConfigMuted;
		
		if (GConfig->GetFloat(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigValueKeyName, ConfigValue, LastEditorConfigParams.ConfigFileName))
		{
			GConfig->RemoveKey(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigValueKeyName, LastEditorConfigParams.ConfigFileName);
			GConfig->SetFloat(*ConfigParams.ConfigSection, *ConfigParams.ConfigValueKeyName, ConfigValue, ConfigParams.ConfigFileName);
		}
		if (GConfig->GetBool(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigMuteKeyName, bConfigMuted, LastEditorConfigParams.ConfigFileName))
		{
			GConfig->RemoveKey(*LastEditorConfigParams.ConfigSection, *LastEditorConfigParams.ConfigMuteKeyName, LastEditorConfigParams.ConfigFileName);
			GConfig->SetBool(*ConfigParams.ConfigSection, *ConfigParams.ConfigMuteKeyName, bConfigMuted, ConfigParams.ConfigFileName);
		}
		
		LastEditorConfigParams = ConfigParams;
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSMutableFloatSettingItem, EditorPreviewValue))
	{
		SetValue(EditorPreviewValue);
	}
	else if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSMutableFloatSettingItem, bEditorPreviewMuteState))
	{
		SetIsMuted(bEditorPreviewMuteState);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSMutableFloatSettingItem::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	LastEditorConfigParams = ConfigParams;
#endif
}

bool UVSMutableFloatSettingItem::IsValueValid_Implementation() const
{
	if (FMath::IsNearlyEqual(CurrentValue, MuteStateValue) && !bCurrentIsMuted) return false;
	
	return Super::IsValueValid_Implementation();
}

void UVSMutableFloatSettingItem::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;

	if (FMath::IsNearlyEqual(CurrentValue, MuteStateValue) && !bCurrentIsMuted)
	{
		SetIsMuted(true);
	}
}

void UVSMutableFloatSettingItem::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	Super::SetToBySource_Implementation(ValueSource);

	SetValue(GetNonMutedValue(ValueSource));
	SetIsMuted(GetIsMuted(ValueSource));
}

bool UVSMutableFloatSettingItem::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!FMath::IsNearlyEqual(CurrentValue, GetNonMutedValue(ValueSource))) return false;
	return bCurrentIsMuted == GetIsMuted(ValueSource);
}

void UVSMutableFloatSettingItem::Load_Implementation()
{
	Super::Load_Implementation();
	
	float ConfigValue;
	bool bConfigMuted;
		
	if (GConfig->GetFloat(*ConfigParams.ConfigSection, *ConfigParams.ConfigValueKeyName, ConfigValue, ConfigParams.ConfigFileName))
	{
		SetValue(ConfigValue);
	}
	if (GConfig->GetBool(*ConfigParams.ConfigSection, *ConfigParams.ConfigMuteKeyName, bConfigMuted, ConfigParams.ConfigFileName))
	{
		SetIsMuted(bConfigMuted);
	}
}

void UVSMutableFloatSettingItem::Save_Implementation()
{
	Super::Save_Implementation();
	
	GConfig->SetFloat(*ConfigParams.ConfigSection, *ConfigParams.ConfigValueKeyName, GetNonMutedValue(EVSSettingItemValueSource::System), ConfigParams.ConfigFileName);
	GConfig->SetBool(*ConfigParams.ConfigSection, *ConfigParams.ConfigMuteKeyName, GetIsMuted(EVSSettingItemValueSource::System), ConfigParams.ConfigFileName);
}

void UVSMutableFloatSettingItem::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	ConfirmedValue = CurrentValue;
	bConfirmedIsMuted = bCurrentIsMuted;
}

void UVSMutableFloatSettingItem::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

#if WITH_EDITOR
	EditorPreviewValue = GetValue(EVSSettingItemValueSource::System);
	EditorPreviewText = ValueMuteToText(GetValue(EVSSettingItemValueSource::System), GetIsMuted(EVSSettingItemValueSource::System));
#endif
}

float UVSMutableFloatSettingItem::GetValue(EVSSettingItemValueSource::Type Source) const
{
	if (GetIsMuted(Source))
	{
		return MuteStateValue;
	}

	return GetNonMutedValue(Source);
}

float UVSMutableFloatSettingItem::GetNonMutedValue_Implementation(EVSSettingItemValueSource::Type Source) const
{
	switch (Source)
	{
	case EVSSettingItemValueSource::Default:
		return 0.f;
		
	case EVSSettingItemValueSource::Game:
	case EVSSettingItemValueSource::System:
		return CurrentValue;
		
	case EVSSettingItemValueSource::Confirmed:
		return ConfirmedValue;
		
	default: ;
	}

	return 0.f;
}

void UVSMutableFloatSettingItem::SetValue(float Value)
{
	const float PrevValue = CurrentValue;
	const bool PrevMuted = bCurrentIsMuted;
	CurrentValue = Value;

#if WITH_EDITOR
	EditorPreviewValue = CurrentValue;
#endif
	
	/** Ensure same value mute. */
	if (FMath::IsNearlyEqual(CurrentValue, MuteStateValue))
	{
		if (!GetIsMuted(EVSSettingItemValueSource::System))
		{
			bCurrentIsMuted = true;

#if WITH_EDITOR
			bEditorPreviewMuteState = bCurrentIsMuted;
#endif
		}
	}
	
	if (!IsValueValid())
	{
		Validate();
	}
	else if (
		!FMath::IsNearlyEqual(CurrentValue, PrevValue)
		|| PrevMuted != bCurrentIsMuted)
	{
		NotifyValueUpdated(true);
	}
}

void UVSMutableFloatSettingItem::SetIsMuted(bool bMuted)
{
	const bool PrevMuted = bCurrentIsMuted;
	bCurrentIsMuted = bMuted;

#if WITH_EDITOR
	bEditorPreviewMuteState = bCurrentIsMuted;
#endif
	
	if (!IsValueValid())
	{
		Validate();
	}
	else if (bCurrentIsMuted != PrevMuted)
	{
		NotifyValueUpdated(true);
	}
}

FText UVSMutableFloatSettingItem::ValueMuteToText(float Value, bool bIsMuted)
{
	if (bIsMuted)
	{
		if (!DisplayMutedText.IsEmpty())
		{
			return DisplayMutedText;
		}
	}
	
	const FText& ValueText = UKismetTextLibrary::Conv_DoubleToText(
		Value * DisplayValueMultiplier, HalfToZero,
		false, true,
		1, 324,
		DisplayFractionDigitRange.X, DisplayFractionDigitRange.Y);

	return FText::Format(DisplayTextFormat, ValueText);
}

bool UVSMutableFloatSettingItem::GetIsMuted_Implementation(EVSSettingItemValueSource::Type Source) const
{
	switch (Source)
	{
	case EVSSettingItemValueSource::Default:
		return false;
		
	case EVSSettingItemValueSource::Game:
	case EVSSettingItemValueSource::System:
		return bCurrentIsMuted;
		
	case EVSSettingItemValueSource::Confirmed:
		return bConfirmedIsMuted;
		
	default: ;
	}

	return false;
}

#if WITH_EDITOR
bool UVSMutableFloatSettingItem::EditorAllowChangingConfigParams_Implementation() const
{
	return true;
}

bool UVSMutableFloatSettingItem::EditorAllowChangingEditorPreviewValue_Implementation() const
{
	return true;
}

bool UVSMutableFloatSettingItem::EditorAllowChangingEditorPreviewMuteState_Implementation() const
{
	return true;
}
#endif

