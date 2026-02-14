// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "VSPrivablic.h"
#include "VSSettingSubsystem.h"
#include "VSSettingSystemConfig.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/Math/VSMath.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeX, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeY, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenHeight, int32);

UVSSettingItem_ScreenResolution::UVSSettingItem_ScreenResolution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DisableInWindowedFullscreenMode(true)
	, bCheckForCommandLineOverrides(false)
{
	SetValueType(EVSCommonSettingValueType::String);
	
	ItemTag = EVSSettingItem::Video::ScreenResolution;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.ScreenResolution", "DisplayName", "Screen Resolution");
	ConfigSettings.Section = FString("/Script/Engine.GameUserSettings");
	ConfigSettings.AdditionalNamedKeys.Add("ResolutionSizeX", "ResolutionSizeX");
	ConfigSettings.AdditionalNamedKeys.Add("ResolutionSizeY", "ResolutionSizeY");
	ConfigSettings.AdditionalNamedKeys.Add("LastUserConfirmedResolutionSizeX", "LastUserConfirmedResolutionSizeX");
	ConfigSettings.AdditionalNamedKeys.Add("LastUserConfirmedResolutionSizeX", "LastUserConfirmedResolutionSizeX");
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

void UVSSettingItem_ScreenResolution::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	FCoreDelegates::OnSystemResolutionChanged.AddUObject(this, &UVSSettingItem_ScreenResolution::OnSystemResolutionChanged);
}

void UVSSettingItem_ScreenResolution::Uninitialize_Implementation()
{
	FCoreDelegates::OnSystemResolutionChanged.RemoveAll(this);

	if (UVSSettingSubsystem::Get())
	{
		if (UVSSettingItemBase* SettingItem = UVSSettingSubsystem::Get()->GetSettingItemByTag(EVSSettingItem::Video::WindowMode))
		{
			SettingItem->OnUpdated_Native.RemoveAll(this);
		}
	}
	
	Super::Uninitialize_Implementation();
}

void UVSSettingItem_ScreenResolution::Load_Implementation()
{
	Super::Load_Implementation();

	FIntPoint LoadResolution = FIntPoint::ZeroValue;
	bool bLoaded = false;
	bLoaded = GConfig->GetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("ResolutionSizeX"), LoadResolution.X, *ConfigSettings.FileName);
	bLoaded = GConfig->GetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("ResolutionSizeY"), LoadResolution.Y, *ConfigSettings.FileName) || bLoaded;
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
		TEnumAsByte<EWindowMode::Type> WindowMode = GSystemResolution.WindowMode;
		if (IConsoleVariable* CVarSetRes = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SetRes"), false))
		{
			FIntPoint OriginalResolution;
			UVSPlatformLibrary::ParseSetRes(CVarSetRes->GetString(), OriginalResolution, WindowMode);
		}

		if (WindowMode == EWindowMode::WindowedFullscreen && DisableInWindowedFullscreenMode)
		{
			return;
		}

		FCoreDelegates::OnSystemResolutionChanged.RemoveAll(this);
		
		FIntPoint ScreenResolutionToApply = GetScreenResolution(EVSSettingItemValueSource::System);
		if (WindowMode == EWindowMode::WindowedFullscreen)
		{
			TArray<FIntPoint> ScreenResolutions;
			UKismetSystemLibrary::GetSupportedFullscreenResolutions(ScreenResolutions);
			if (ScreenResolutions.Num())
			{
				ScreenResolutionToApply = ScreenResolutions.Last();
			}
		}
		if (bCheckForCommandLineOverrides)
		{
			EWindowMode::Type TempWindowMode = WindowMode;
			UGameEngine::ConditionallyOverrideSettings(ScreenResolutionToApply.X, ScreenResolutionToApply.Y, TempWindowMode);
		}
		FSystemResolution::RequestResolutionChange(ScreenResolutionToApply.X, ScreenResolutionToApply.Y, WindowMode);
		
		FCoreDelegates::OnSystemResolutionChanged.AddUObject(this, &UVSSettingItem_ScreenResolution::OnSystemResolutionChanged);
	}
#endif
}

void UVSSettingItem_ScreenResolution::Confirm_Implementation()
{
	Super::Confirm_Implementation();

	if (GEngine && GEngine->GetGameUserSettings())
	{
		const FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenWidth) = ScreenResolution.X;
		VS_PRIVABLIC_MEMBER(GEngine->GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenHeight) = ScreenResolution.Y;
	}
}

void UVSSettingItem_ScreenResolution::Save_Implementation()
{
	Super::Save_Implementation();

	if (GEngine)
	{
		const FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("ResolutionSizeX"), ScreenResolution.X, *ConfigSettings.FileName);
		GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("ResolutionSizeY"), ScreenResolution.Y, *ConfigSettings.FileName);

		if (GEngine->GetGameUserSettings())
		{
			GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("LastUserConfirmedResolutionSizeX"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().X, *ConfigSettings.FileName);
			GConfig->SetInt(*ConfigSettings.Section, *ConfigSettings.AdditionalNamedKeys.FindRef("LastUserConfirmedResolutionSizeX"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().Y, *ConfigSettings.FileName);
		}

		GConfig->Flush(false, GGameUserSettingsIni);
	}
}

bool UVSSettingItem_ScreenResolution::IsValueValid_Implementation() const
{
	const FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	if (ScreenResolution.X <= 0 || ScreenResolution.Y <= 0) return false;
	
	return true;
}

void UVSSettingItem_ScreenResolution::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	
	FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	ScreenResolution.X = FMath::Max(ScreenResolution.X, 0);
	ScreenResolution.Y = FMath::Max(ScreenResolution.Y, 0);

	if (ScreenResolution == FIntPoint::ZeroValue)
	{
		TArray<FIntPoint> ScreenResolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(ScreenResolutions);
		if (ScreenResolutions.Num())
		{
			ScreenResolution = ScreenResolutions.Last();
		}
		if (ScreenResolution != FIntPoint::ZeroValue)
		{
			SetScreenResolution(ScreenResolution);
		}
	}
}

void UVSSettingItem_ScreenResolution::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();
	
#if WITH_EDITORONLY_DATA
	EditorPreviewScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
#endif
	
	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GameUserSettings)
	{
		const FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
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
			if (IConsoleVariable* CVarSetRes = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SetRes"), false))
			{
				TEnumAsByte<EWindowMode::Type> WindowMode;
				FIntPoint Resolution;
				UVSPlatformLibrary::ParseSetRes(CVarSetRes->GetString(), Resolution, WindowMode);
				return Resolution.ToString();
			}
			return FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY).ToString();

		default: ;
			break;
		}
	}
		
	return Super::GetStringValue_Implementation(ValueSource);
}

FText UVSSettingItem_ScreenResolution::ValueStringToText_Implementation(const FString& String) const
{
	/** [Generated by ChatGPT] */
	const FIntPoint ScreenResolution = StringToScreenResolution(String);
	if (ScreenResolution == FIntPoint::ZeroValue)
	{
		return UVSSettingSystemConfig::Get()->SystemDefaultText;
	}

	FString Left  = FString::FromInt(ScreenResolution.X);
	FString Right = FString::FromInt(ScreenResolution.Y);
	
	/** Figure space. */
	// static int32 FieldWidth = 6;
	//
	// auto PadLeftTo = [&] (FString& InOut, int32 Width)
	// {
	// 	const int32 PadCount = Width - InOut.Len();
	// 	if (PadCount > 0)
	// 	{
	// 		InOut = FString::ChrN(PadCount, PadChar) + InOut;
	// 	}
	// };
	//
	// auto PadRightTo = [&] (FString& InOut, int32 Width)
	// {
	// 	const int32 PadCount = Width - InOut.Len();
	// 	if (PadCount > 0)
	// 	{
	// 		InOut = InOut + FString::ChrN(PadCount, PadChar);
	// 	}
	// };
	//
	// PadLeftTo(Left, FieldWidth);
	// PadRightTo(Right, FieldWidth);
	
	/** Figure Space (U+2007): typically same width as digits. */
	static TCHAR PadChar = 0x2007;
	FString Result = Left + FString::Chr(PadChar) + TEXT("x") + FString::Chr(PadChar) + Right;
	if (Right.Len() > Left.Len())
	{
		Result += FString::ChrN(Right.Len() - Left.Len(), PadChar);
	}
	
	return FText::FromString(Result);
}

void UVSSettingItem_ScreenResolution::SetScreenResolution(const FIntPoint& InResolution)
{
	SetStringValue(InResolution.ToString());
}

FIntPoint UVSSettingItem_ScreenResolution::GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const
{
	const FString ValueString = GetStringValue(ValueSource);
	return StringToScreenResolution(ValueString);
}

FIntPoint UVSSettingItem_ScreenResolution::StringToScreenResolution(const FString& String) const
{
	FIntPoint Result(0, 0);
	FParse::Value(*String, TEXT("X="), Result.X);
	FParse::Value(*String, TEXT("Y="), Result.Y);
	return Result;
}

void UVSSettingItem_ScreenResolution::OnSystemResolutionChanged(uint32 ResX, uint32 ResY)
{
	FCoreDelegates::OnSystemResolutionChanged.RemoveAll(this);
	NotifyValueExternChanged(false);
	FCoreDelegates::OnSystemResolutionChanged.AddUObject(this, &UVSSettingItem_ScreenResolution::OnSystemResolutionChanged);
}
