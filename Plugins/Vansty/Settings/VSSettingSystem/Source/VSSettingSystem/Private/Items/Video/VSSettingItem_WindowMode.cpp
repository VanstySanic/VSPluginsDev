// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_WindowMode.h"
#include "VSPrivablic.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, FullscreenMode, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastConfirmedFullscreenMode, int32);

UVSSettingItem_WindowMode::UVSSettingItem_WindowMode(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::Integer);
	
	ItemTag = EVSSettingItem::Video::WindowMode;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.WindowMode", "DisplayName", "Window Mode");
	ConfigParams.ConfigSection = FString("/Script/Engine.GameUserSettings");
	ConfigParams.ConfigKeyName = FString("FullscreenMode");
}

void UVSSettingItem_WindowMode::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorPreviewWindowMode = GetWindowMode(EVSSettingItemValueSource::System);
#endif
}

#if WITH_EDITOR
void UVSSettingItem_WindowMode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_WindowMode, EditorPreviewWindowMode))
	{
		SetIntegerValue(EditorPreviewWindowMode);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_WindowMode::Apply_Implementation()
{
	Super::Apply_Implementation();

#if !UE_SERVER
	if (!FPlatformProperties::HasFixedResolution())
	{
		EWindowMode::Type WindowModeToApply = GetWindowMode(EVSSettingItemValueSource::System);
		UGameEngine::ConditionallyOverrideSettings(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
		FSystemResolution::RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
	}
#endif
}

void UVSSettingItem_WindowMode::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	if (GEngine && GEngine->GameUserSettings)
	{
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, LastConfirmedFullscreenMode) = GetIntegerValue();
	}
}

void UVSSettingItem_WindowMode::Save_Implementation()
{
	Super::Save_Implementation();

	GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("LastConfirmedFullscreenMode"), GetWindowMode(EVSSettingItemValueSource::Confirmed), ConfigParams.ConfigFileName);
	GConfig->Flush(false, ConfigParams.ConfigFileName);
}

bool UVSSettingItem_WindowMode::IsValueValid_Implementation() const
{
	const EWindowMode::Type WindowMode = GetWindowMode(EVSSettingItemValueSource::System);
	return FVSMath::IsInRange(WindowMode, 0, EWindowMode::NumWindowModes - 1);
}

void UVSSettingItem_WindowMode::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	const EWindowMode::Type PrevWindowMode = GetWindowMode(EVSSettingItemValueSource::Game);
	SetIntegerValue(FMath::Clamp(PrevWindowMode, 0, EWindowMode::NumWindowModes - 1));
}

void UVSSettingItem_WindowMode::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

#if WITH_EDITORONLY_DATA
	EditorPreviewWindowMode = GetWindowMode(EVSSettingItemValueSource::System);
	SetEditorPreviewValueString(GetStringValue(EVSSettingItemValueSource::System));
#endif

	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GameUserSettings)
	{
		const EWindowMode::Type WindowMode = GetWindowMode(EVSSettingItemValueSource::Game);
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, FullscreenMode) = WindowMode;
	}
}

int32 UVSSettingItem_WindowMode::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return Super::GetIntegerValue_Implementation(ValueSource);

	if (ValueType == EVSCommonSettingValueType::Integer)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return GEngine->GetGameUserSettings()->GetDefaultWindowMode();
			
		case EVSSettingItemValueSource::Game:
			return GEngine->GetGameUserSettings()->GetFullscreenMode();
			
		default: ;
		}
	}
		
	return Super::GetIntegerValue_Implementation(ValueSource);
}

void UVSSettingItem_WindowMode::SetWindowMode(EWindowMode::Type InWindowMode)
{
	SetIntegerValue(InWindowMode);
}

EWindowMode::Type UVSSettingItem_WindowMode::GetWindowMode(EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<EWindowMode::Type>(GetIntegerValue(ValueSource));
}