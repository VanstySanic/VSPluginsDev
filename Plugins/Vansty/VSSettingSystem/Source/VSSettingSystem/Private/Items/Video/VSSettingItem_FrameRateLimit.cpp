// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_FrameRateLimit.h"
#include "VSSettingSystemConfig.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_FrameRateLimit::UVSSettingItem_FrameRateLimit(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Video_FrameRateLimit;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.FrameRateLimit", "Frame Rate Limit");

	DesiredValueStep = 1.f;
	DesiredValueRange = FVector2D(30.f, 360.f);
	
	bUseGeneratedOptions = false;
	CustomOptions = TArray<float> { 30.f, 60.f, 75.f, 90.f, 120.f, 144.f, 240.f, 360.f, 0.f };
	Algo::Reverse(CustomOptions);

	NamedValues.Emplace(0.f, UVSSettingSystemConfig::Get()->NoLimitsText);
	DisplayFractionNum = 0.f;
	ContentTextFormat = FText::FromString("{0} FPS");
}

void UVSSettingItem_FrameRateLimit::Load_Implementation()
{
	float LoadFrameRateLimit = 0.f;
	if (GConfig->GetFloat(TEXT("/Script/Engine.GameUserSettings"), TEXT("FrameRateLimit"), LoadFrameRateLimit, GGameUserSettingsIni))
	{
		SetFrameRateLimit(LoadFrameRateLimit);
	}
}

void UVSSettingItem_FrameRateLimit::Apply_Implementation()
{
	if (GEngine)
	{
		GEngine->SetMaxFPS(GetFrameRateLimit(EVSSettingItemValueSource::Settings));
	}
}

void UVSSettingItem_FrameRateLimit::Confirm_Implementation()
{
	LastConfirmedFrameRateLimit = GetFrameRateLimit(EVSSettingItemValueSource::Settings);
}

void UVSSettingItem_FrameRateLimit::Save_Implementation()
{
	GConfig->SetFloat(TEXT("/Script/Engine.GameUserSettings"), TEXT("FrameRateLimit"), GetFrameRateLimit(EVSSettingItemValueSource::Settings), GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_FrameRateLimit::SetValue_Implementation(float InValue)
{
	SetFrameRateLimit(InValue);
}

float UVSSettingItem_FrameRateLimit::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetFrameRateLimit(ValueSource);
}

void UVSSettingItem_FrameRateLimit::SetFrameRateLimit(float InLimit)
{
	if (FMath::IsNearlyEqual(GetFrameRateLimit(EVSSettingItemValueSource::Settings), InLimit)) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetFrameRateLimit(InLimit);
	NotifyValueUpdate();
}

float UVSSettingItem_FrameRateLimit::GetFrameRateLimit(EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return 0.f;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return 0.f;
			
	case EVSSettingItemValueSource::Current:
		return GEngine ? GEngine->GetMaxFPS() : 0.f;
			
	case EVSSettingItemValueSource::Settings:
		return GameUserSettings->GetFrameRateLimit();

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedFrameRateLimit;
		
	default:
		return 0.f;
	}
}
