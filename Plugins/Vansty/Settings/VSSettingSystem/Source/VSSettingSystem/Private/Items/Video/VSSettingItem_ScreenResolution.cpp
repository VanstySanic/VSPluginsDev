// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "VSPrivablic.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeX, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeY, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenHeight, int32);

UVSSettingItem_ScreenResolution::UVSSettingItem_ScreenResolution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::String);
	
	ItemTag = EVSSettingItem::Video::ScreenResolution;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.ScreenResolution", "DisplayName", "Screen Resolution");
	ConfigParams.ConfigSection = FString("/Script/Engine.GameUserSettings");
	ConfigParams.ConfigKeyName = FString("ResolutionSize");
	ConfigParams.bAutoDefaultConfig = false;
}

void UVSSettingItem_ScreenResolution::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorPreviewScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
#endif
}

#if WITH_EDITOR
void UVSSettingItem_ScreenResolution::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_ScreenResolution, EditorPreviewScreenResolution))
	{
		SetStringValue(EditorPreviewScreenResolution.ToString());
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_ScreenResolution::Load_Implementation()
{
	Super::Load_Implementation();

	FIntPoint LoadResolution = FIntPoint::ZeroValue;
	bool bLoaded = false;
	bLoaded = GConfig->GetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeX"), LoadResolution.X, *ConfigParams.ConfigKeyName);
	bLoaded = GConfig->GetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeY"), LoadResolution.Y, *ConfigParams.ConfigKeyName) || bLoaded;
	if (bLoaded)
	{
		SetScreenResolution(LoadResolution);
	}
}

void UVSSettingItem_ScreenResolution::Apply_Implementation()
{
	Super::Apply_Implementation();

#if !UE_SERVER
	if (!FPlatformProperties::HasFixedResolution())
	{
		FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		UGameEngine::ConditionallyOverrideSettings(ScreenResolution.X, ScreenResolution.Y, GSystemResolution.WindowMode);
		FSystemResolution::RequestResolutionChange(ScreenResolution.X, ScreenResolution.Y, GSystemResolution.WindowMode);
	}
#endif
}

void UVSSettingItem_ScreenResolution::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	if (GEngine && GEngine->GameUserSettings)
	{
		UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
		const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenWidth) = ScreenResolution.X;
		VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenHeight) = ScreenResolution.Y;
	}
}

void UVSSettingItem_ScreenResolution::Save_Implementation()
{
	Super::Save_Implementation();

	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeX"), ScreenResolution.X, *ConfigParams.ConfigKeyName);
	GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeY"), ScreenResolution.Y, *ConfigParams.ConfigKeyName);
	
	if (GEngine)
	{
		GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("LastUserConfirmedResolutionSizeX"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().X, *ConfigParams.ConfigKeyName);
		GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("LastUserConfirmedResolutionSizeY"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().Y, *ConfigParams.ConfigKeyName);
	}

	GConfig->Flush(false, GGameUserSettingsIni);
}

bool UVSSettingItem_ScreenResolution::IsValueValid_Implementation() const
{
	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	return ScreenResolution.X >= 0 && ScreenResolution.Y >= 0;
}

void UVSSettingItem_ScreenResolution::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	
	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	FIntPoint ValidatedResolution = ScreenResolution;
	ValidatedResolution.X = FMath::Max(ValidatedResolution.X, 0);
	ValidatedResolution.Y = FMath::Max(ValidatedResolution.Y, 0);

	SetScreenResolution(ValidatedResolution);
}

void UVSSettingItem_ScreenResolution::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

#if WITH_EDITORONLY_DATA
	EditorPreviewScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	SetEditorPreviewValueString(GetStringValue(EVSSettingItemValueSource::System));
#endif

	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GameUserSettings)
	{
		const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, ResolutionSizeX) = ScreenResolution.X;
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, ResolutionSizeY) = ScreenResolution.Y;
	}
}

FString UVSSettingItem_ScreenResolution::GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return Super::GetStringValue_Implementation(ValueSource);

	if (ValueType == EVSCommonSettingValueType::String)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return GEngine->GetGameUserSettings()->GetDefaultResolution().ToString();
			
		case EVSSettingItemValueSource::Game:
			return GEngine->GetGameUserSettings()->GetScreenResolution().ToString();
			
		default: ;
		}
	}
		
	return Super::GetStringValue_Implementation(ValueSource);
}

void UVSSettingItem_ScreenResolution::SetScreenResolution(const FIntPoint& InResolution)
{
	SetStringValue(InResolution.ToString());
}

FIntPoint UVSSettingItem_ScreenResolution::GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const
{
	const FString& ValueString = GetStringValue(ValueSource);
	FIntPoint Result(0, 0);
	FParse::Value(*ValueString, TEXT("X="), Result.X);
	FParse::Value(*ValueString, TEXT("Y="), Result.Y);
	return Result;
}
