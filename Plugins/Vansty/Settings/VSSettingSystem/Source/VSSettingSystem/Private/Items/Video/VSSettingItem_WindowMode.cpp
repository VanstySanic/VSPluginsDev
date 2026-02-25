// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_WindowMode.h"
#include "VSPrivablic.h"
#include "VSSettingSystemConfig.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/Math/VSMath.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, FullscreenMode, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastConfirmedFullscreenMode, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, PreferredFullscreenMode, int32);

UVSSettingItem_WindowMode::UVSSettingItem_WindowMode(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
	, bCheckForCommandLineOverrides(false)
{
	SetValueType(EVSCommonSettingValueType::Integer);

	ItemTag = EVSSettingItem::Video::WindowMode;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.WindowMode", "DisplayName", "Window Mode");
	ConfigSettings.Section = "/Script/Engine.GameUserSettings";
	ConfigSettings.PrimaryKey = "FullscreenMode";
	ConfigSettings.AdditionalNamedKeys.Add("LastConfirmedFullscreenMode", "LastConfirmedFullscreenMode");
	ConfigSettings.AdditionalNamedKeys.Add("PreferredFullscreenMode", "PreferredFullscreenMode");

	ExternalChangeActions.Empty();
}

#if WITH_EDITOR
void UVSSettingItem_WindowMode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_WindowMode, EditorPreviewWindowMode))
	{
		SetWindowMode(EditorPreviewWindowMode);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_WindowMode::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	FViewport::ViewportResizedEvent.AddUObject(this, &UVSSettingItem_WindowMode::OnViewportResized);
}

void UVSSettingItem_WindowMode::Uninitialize_Implementation()
{
	FViewport::ViewportResizedEvent.RemoveAll(this);

	Super::Uninitialize_Implementation();
}

void UVSSettingItem_WindowMode::Apply_Implementation()
{
	Super::Apply_Implementation();

#if !UE_SERVER
	FIntPoint ScreenResolution = FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY);
	if (IConsoleVariable* CVarSetRes = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SetRes"), false))
	{
		TEnumAsByte<EWindowMode::Type> OriginalWindowMode;
		UVSPlatformLibrary::ParseSetRes(CVarSetRes->GetString(), ScreenResolution, OriginalWindowMode);
	}
	
	EWindowMode::Type WindowModeToApply = GetWindowMode(EVSSettingItemValueSource::System);
	if (WindowModeToApply == EWindowMode::WindowedFullscreen)
	{
		TArray<FIntPoint> ScreenResolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(ScreenResolutions);
		if (ScreenResolutions.Num())
		{
			ScreenResolution = ScreenResolutions.Last();
		}
	}
	if (bCheckForCommandLineOverrides)
	{
		FIntPoint TempResolution = ScreenResolution;
		UGameEngine::ConditionallyOverrideSettings(TempResolution.X, TempResolution.Y, WindowModeToApply);
	}
	FSystemResolution::RequestResolutionChange(ScreenResolution.X, ScreenResolution.Y, WindowModeToApply);

	if (WindowModeToApply == EWindowMode::Fullscreen || WindowModeToApply == EWindowMode::WindowedFullscreen)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FullScreenMode"), false))
		{
			CVar->SetWithCurrentPriority(WindowModeToApply == EWindowMode::Fullscreen ? 0 : 1);
		}
	}
#endif
}

void UVSSettingItem_WindowMode::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	if (GEngine && GEngine->GetGameUserSettings())
	{
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, LastConfirmedFullscreenMode) = GetIntegerValue();
	}
}

void UVSSettingItem_WindowMode::Save_Implementation()
{
	Super::Save_Implementation();
	
	GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("LastConfirmedFullscreenMode"), GetWindowMode(EVSSettingItemValueSource::Confirmed), ConfigSettings.FileName);
	GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("PreferredFullscreenMode"), GetWindowMode(EVSSettingItemValueSource::Confirmed), ConfigSettings.FileName);

	GConfig->Flush(false, *ConfigSettings.FileName);
}

bool UVSSettingItem_WindowMode::IsValueValid_Implementation() const
{
	const EWindowMode::Type WindowMode = GetWindowMode(EVSSettingItemValueSource::System);
	if (!FVSMath::IsInRange(WindowMode, 0, EWindowMode::NumWindowModes - 1)) return false;

	if (WindowMode == EWindowMode::Windowed && !FPlatformProperties::SupportsWindowedMode()) return false;
	
	return true;
}

void UVSSettingItem_WindowMode::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;

	EWindowMode::Type WindowMode = GetWindowMode(EVSSettingItemValueSource::System);
	WindowMode = (EWindowMode::Type)FMath::Clamp(WindowMode, 0, EWindowMode::NumWindowModes - 1);
	
	if (WindowMode == EWindowMode::Windowed && !FPlatformProperties::SupportsWindowedMode())
	{
		WindowMode = GetWindowMode(EVSSettingItemValueSource::Confirmed);
		if (WindowMode == EWindowMode::Windowed)
		{
			WindowMode = GetWindowMode(EVSSettingItemValueSource::Game);
		}
	}
	
	SetIntegerValue(WindowMode);
}

void UVSSettingItem_WindowMode::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();
	
#if WITH_EDITORONLY_DATA
	EditorPreviewWindowMode = GetWindowMode(EVSSettingItemValueSource::System);
#endif
	
	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GetGameUserSettings())
	{
		const EWindowMode::Type WindowMode = GetWindowMode(EVSSettingItemValueSource::System);
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, FullscreenMode) = WindowMode;
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, PreferredFullscreenMode) = WindowMode == EWindowMode::Fullscreen ? 1 : 0;
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
			return GEngine->GetGameUserSettings() ? GEngine->GameUserSettings->GetDefaultWindowMode() : EWindowMode::WindowedFullscreen;
			
		case EVSSettingItemValueSource::Game:
			if (IConsoleVariable* CVarSetRes = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SetRes"), false))
			{
				TEnumAsByte<EWindowMode::Type> WindowMode;
				FIntPoint Resolution;
				UVSPlatformLibrary::ParseSetRes(CVarSetRes->GetString(), Resolution, WindowMode);
				return WindowMode;
			}
			return GSystemResolution.WindowMode;
			
		default: ;
		}
	}
		
	return Super::GetIntegerValue_Implementation(ValueSource);
}

FText UVSSettingItem_WindowMode::ValueStringToText_Implementation(const FString& String) const
{
	static TMap<FString, FText> OutMap = TMap<FString, FText>
	{
		{ "0", UVSSettingSystemConfig::Get()->WindowModeDisplayNames.FindRef(EWindowMode::Type(0)) },
		{ "1", UVSSettingSystemConfig::Get()->WindowModeDisplayNames.FindRef(EWindowMode::Type(1)) },
		{ "2", UVSSettingSystemConfig::Get()->WindowModeDisplayNames.FindRef(EWindowMode::Type(2)) },
	};
	
	return OutMap.FindRef(String);
}

void UVSSettingItem_WindowMode::SetWindowMode(EWindowMode::Type InWindowMode)
{
	SetIntegerValue(InWindowMode);
}

EWindowMode::Type UVSSettingItem_WindowMode::GetWindowMode(EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<EWindowMode::Type>(GetIntegerValue(ValueSource));
}

void UVSSettingItem_WindowMode::OnViewportResized(FViewport* Viewport, uint32 Value)
{
	FViewport::ViewportResizedEvent.RemoveAll(this);
	NotifyValueExternChanged(false);
	FViewport::ViewportResizedEvent.AddUObject(this, &UVSSettingItem_WindowMode::OnViewportResized);
}
