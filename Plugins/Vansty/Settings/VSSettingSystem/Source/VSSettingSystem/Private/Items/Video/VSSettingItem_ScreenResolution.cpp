// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "VSPrivablic.h"
#include "VSSettingSubsystem.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeX, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, ResolutionSizeY, uint32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenHeight, int32);

UVSSettingItem_ScreenResolution::UVSSettingItem_ScreenResolution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bCheckForCommandLineOverrides(false)
{
	SetValueType(EVSCommonSettingValueType::String);
	
	ItemTag = EVSSettingItem::Video::ScreenResolution;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.ScreenResolution", "DisplayName", "Screen Resolution");
	ConfigParams.ConfigSection = FString("/Script/Engine.GameUserSettings");
	ConfigParams.ConfigKeyName = FString("ResolutionSize");
	ConfigParams.bAutoDefaultConfig = false;
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
		if (UVSSettingItem* SettingItem = UVSSettingSubsystem::Get()->GetSettingItemByTag(EVSSettingItem::Video::WindowMode))
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
	bLoaded = GConfig->GetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeX"), LoadResolution.X, *ConfigParams.ConfigFileName);
	bLoaded = GConfig->GetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeY"), LoadResolution.Y, *ConfigParams.ConfigFileName) || bLoaded;
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
		FCoreDelegates::OnSystemResolutionChanged.RemoveAll(this);

		FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
		if (bCheckForCommandLineOverrides)
		{
			EWindowMode::Type WindowMode = GSystemResolution.WindowMode;
			UGameEngine::ConditionallyOverrideSettings(ScreenResolution.X, ScreenResolution.Y, WindowMode);
		}
		
		FSystemResolution::RequestResolutionChange(ScreenResolution.X, ScreenResolution.Y, GSystemResolution.WindowMode);
		
		FCoreDelegates::OnSystemResolutionChanged.AddUObject(this, &UVSSettingItem_ScreenResolution::OnSystemResolutionChanged);
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
	GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeX"), ScreenResolution.X, *ConfigParams.ConfigFileName);
	GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("ResolutionSizeY"), ScreenResolution.Y, *ConfigParams.ConfigFileName);
	
	if (GEngine && GEngine->GetGameUserSettings())
	{
		GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("LastUserConfirmedResolutionSizeX"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().X, *ConfigParams.ConfigFileName);
		GConfig->SetInt(*ConfigParams.ConfigSection, TEXT("LastUserConfirmedResolutionSizeY"), GEngine->GameUserSettings->GetLastConfirmedScreenResolution().Y, *ConfigParams.ConfigFileName);
	}

	GConfig->Flush(false, GGameUserSettingsIni);
}

bool UVSSettingItem_ScreenResolution::IsValueValid_Implementation() const
{
	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	if (ScreenResolution.X < 0 || ScreenResolution.Y < 0) return false;

	if (bCheckForCommandLineOverrides)
	{
		EWindowMode::Type WindowMode = GSystemResolution.WindowMode;
		FIntPoint OverrideScreenResolution = ScreenResolution;
		UGameEngine::ConditionallyOverrideSettings(OverrideScreenResolution.X, OverrideScreenResolution.Y, WindowMode);
		if (ScreenResolution != OverrideScreenResolution) return false;
	}
	
	return true;
}

void UVSSettingItem_ScreenResolution::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	
	FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
	ScreenResolution.X = FMath::Max(ScreenResolution.X, 0);
	ScreenResolution.Y = FMath::Max(ScreenResolution.Y, 0);

	if (bCheckForCommandLineOverrides)
	{
		EWindowMode::Type WindowMode = GSystemResolution.WindowMode;
		UGameEngine::ConditionallyOverrideSettings(ScreenResolution.X, ScreenResolution.Y, WindowMode);
	}

	SetScreenResolution(ScreenResolution);
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
	FIntPoint ScreenResolution = StringToScreenResolution(String);

	if (FVSMath::VectorHasZeroAxis(ScreenResolution) && GEngine)
	{
		if (GEngine->GameViewport && GEngine->GameViewport->Viewport)
		{
			ScreenResolution = GEngine->GameViewport->Viewport->GetSizeXY();
		}
		else if (UGameUserSettings* GameUserSettings = GEngine->GameUserSettings)
		{
			ScreenResolution = GameUserSettings->GetScreenResolution();
		}
	}

	FString Left  = FString::FromInt(ScreenResolution.X);
	FString Right = FString::FromInt(ScreenResolution.Y);

	/** Figure space. */
	static int32 FieldWidth = 6;

	/** Figure Space (U+2007): typically same width as digits. */
	static TCHAR PadChar = 0x2007;

	auto PadLeftTo = [&](FString& InOut, int32 Width)
	{
		const int32 PadCount = Width - InOut.Len();
		if (PadCount > 0)
		{
			InOut = FString::ChrN(PadCount, PadChar) + InOut;
		}
	};

	auto PadRightTo = [&](FString& InOut, int32 Width)
	{
		const int32 PadCount = Width - InOut.Len();
		if (PadCount > 0)
		{
			InOut = InOut + FString::ChrN(PadCount, PadChar);
		}
	};

	PadLeftTo(Left, FieldWidth);
	PadRightTo(Right, FieldWidth);

	const FString Result = Left + TEXT(" x ") + Right;
	return FText::FromString(Result);
}


#if WITH_EDITOR
void UVSSettingItem_ScreenResolution::EditorPostInitialized_Implementation()
{
	Super::EditorPostInitialized_Implementation();
	
	EditorPreviewScreenResolution = GetScreenResolution(EVSSettingItemValueSource::System);
}
#endif

void UVSSettingItem_ScreenResolution::SetScreenResolution(const FIntPoint& InResolution)
{
	SetStringValue(InResolution.ToString());
}

FIntPoint UVSSettingItem_ScreenResolution::GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const
{
	const FString& ValueString = GetStringValue(ValueSource);
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
