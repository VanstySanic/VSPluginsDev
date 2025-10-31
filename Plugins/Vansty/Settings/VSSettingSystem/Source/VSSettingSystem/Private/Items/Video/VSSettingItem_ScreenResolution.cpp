// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "CommonRotator.h"
#include "CommonTextBlock.h"
#include "VSPrivablic.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSSettingSystemTags.h"

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, DefaultOptions, TArray<FString>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenWidth, int32);
VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastUserConfirmedDesiredScreenHeight, int32);

UVSSettingItem_ScreenResolution::UVSSettingItem_ScreenResolution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Video_ScreenResolution;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.ScreenResolution", "Screen Resolution");
}

void UVSSettingItem_ScreenResolution::Load_Implementation()
{
	FIntPoint LoadResolution = FIntPoint::ZeroValue;
	bool bLoaded = false;
	bLoaded = GConfig->GetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("ResolutionSizeX"), LoadResolution.X, GGameUserSettingsIni);
	bLoaded = GConfig->GetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("ResolutionSizeY"), LoadResolution.Y, GGameUserSettingsIni) || bLoaded;
	if (bLoaded)
	{
		SetScreenResolution(LoadResolution);
	}
}

void UVSSettingItem_ScreenResolution::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	const FIntPoint& SettingResolution = GetScreenResolution(EVSSettingItemValueSource::Settings);
	if (SettingResolution == FIntPoint::ZeroValue)
	{
		SetScreenResolution(GameUserSettings->GetDesktopResolution());
	}
	else
	{
		TArray<FIntPoint> Resolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		if (!Resolutions.Contains(GetScreenResolution(EVSSettingItemValueSource::Settings)))
		{
			SetScreenResolution(GameUserSettings->GetDesktopResolution());
		}
	}
}

void UVSSettingItem_ScreenResolution::Apply_Implementation()
{
#if !UE_SERVER
	if (FPlatformProperties::HasFixedResolution()) return;
	FIntPoint ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::Settings);
	UGameEngine::ConditionallyOverrideSettings(ScreenResolution.X, ScreenResolution.Y, GSystemResolution.WindowMode);
	FSystemResolution::RequestResolutionChange(ScreenResolution.X, ScreenResolution.Y, GSystemResolution.WindowMode);
#endif
}

void UVSSettingItem_ScreenResolution::Confirm_Implementation()
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::Settings);
	VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenWidth) = ScreenResolution.X;
	VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastUserConfirmedDesiredScreenHeight) = ScreenResolution.Y;
}

void UVSSettingItem_ScreenResolution::Save_Implementation()
{
	const FIntPoint& ScreenResolution = GetScreenResolution(EVSSettingItemValueSource::Settings);
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("ResolutionSizeX"), ScreenResolution.X, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("ResolutionSizeX"), ScreenResolution.X, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("LastUserConfirmedResolutionSizeX"), GameUserSettings->GetLastConfirmedScreenResolution().X, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("LastUserConfirmedResolutionSizeY"), GameUserSettings->GetLastConfirmedScreenResolution().Y, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_ScreenResolution::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetScreenResolution(GetScreenResolution(ValueSource));
}

bool UVSSettingItem_ScreenResolution::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetScreenResolution(EVSSettingItemValueSource::Settings) == GetScreenResolution(ValueSource);
}

void UVSSettingItem_ScreenResolution::OnItemValueUpdated_Implementation()
{
	Super::OnItemValueUpdated_Implementation();

	const bool bIsInGame = UVSGameplayLibrary::IsInGame();
	const FText& SettingText = ResolutionToText(GetScreenResolution(EVSSettingItemValueSource::Settings));
	
	/** Update widget values. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Value")))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (bIsInGame) { ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItem_ScreenResolution::OnValueComboBoxStringSelectionChanged); }
			ComboBox->SetSelectedOption(SettingText.ToString());
			if (bIsInGame) { ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItem_ScreenResolution::OnValueComboBoxStringSelectionChanged); }
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (bIsInGame) { Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItem_ScreenResolution::OnValueRotatorRotatedWithDirection); }
			for (int i = 0; i < VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels).Num(); i++)
			{
				if (SettingText.EqualTo(VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels)[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}
			if (bIsInGame) { Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItem_ScreenResolution::OnValueRotatorRotatedWithDirection); }
		}
	}

	/** Refresh content. */
	for (UWidget* Widget : GetBoundWidgetsOfType(FName("Content")))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItem_ScreenResolution::BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	Super::BindWidgetInternal_Implementation(Widget, TypeName);

	const FText& SettingText = ResolutionToText(GetScreenResolution(EVSSettingItemValueSource::Settings));
	
	if (TypeName == FName("Value"))
	{
		TArray<FIntPoint> Resolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		
		/** Generate options. */
		TArray<FText> OptionTexts;
		for (const FIntPoint& Resolution : Resolutions)
		{
			OptionTexts.Add(ResolutionToText(Resolution));
		}

		if (bOptionsHighToLow)
		{
			Algo::Reverse(OptionTexts);
		}
		
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			ComboBox->ClearOptions();
			for (const FText& Option : OptionTexts)
			{
				ComboBox->AddOption(Option.ToString());
			}

			/** TODO Directly set dosen't show text. Don't know why. */
			ComboBox->AddOption(FString("TempOption"));
			ComboBox->SetSelectedOption(FString("TempOption"));
			ComboBox->RefreshOptions();
			ComboBox->SetSelectedOption(SettingText.ToString());
			ComboBox->RemoveOption(FString("TempOption"));
			ComboBox->RefreshOptions();

			if (!UVSGameplayLibrary::IsInGame())
			{
				VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Empty();
				for (const FText& Option : OptionTexts)
				{
					VS_PRIVABLIC_MEMBER(ComboBox, UComboBoxString, DefaultOptions).Add(Option.ToString());
				}
			}
			else
			{
				ComboBox->OnSelectionChanged.AddDynamic(this, &UVSSettingItem_ScreenResolution::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			Rotator->PopulateTextLabels(OptionTexts);

			/** Init the original item. */
			for (int i = 0; i < OptionTexts.Num(); ++i)
			{
				if (SettingText.EqualTo(OptionTexts[i]))
				{
					Rotator->SetSelectedItem(i);
					break;
				}
			}

			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.AddDynamic(this, &UVSSettingItem_ScreenResolution::OnValueRotatorRotatedWithDirection);
			}
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(SettingText);
		}
	}
}

void UVSSettingItem_ScreenResolution::UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName)
{
	if (TypeName == FName("Value"))
	{
		if (UComboBoxString* ComboBox = Cast<UComboBoxString>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				ComboBox->OnSelectionChanged.RemoveDynamic(this, &UVSSettingItem_ScreenResolution::OnValueComboBoxStringSelectionChanged);
			}
		}
		else if (UCommonRotator* Rotator = Cast<UCommonRotator>(Widget))
		{
			if (UVSGameplayLibrary::IsInGame())
			{
				Rotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSSettingItem_ScreenResolution::OnValueRotatorRotatedWithDirection);
			}
		}
	}
	
	Super::UnbindWidgetInternal_Implementation(Widget, TypeName);
}

void UVSSettingItem_ScreenResolution::OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SetScreenResolution(TextToResolution(FText::FromString(SelectedItem)));
}

void UVSSettingItem_ScreenResolution::OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir)
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);

	if (Resolutions.IsValidIndex(NewValue))
	{
		SetScreenResolution(Resolutions[NewValue]);
	}
}

void UVSSettingItem_ScreenResolution::SetScreenResolution(const FIntPoint& InResolution)
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	
	if (InResolution == GetScreenResolution(EVSSettingItemValueSource::Settings)) return;

	FIntPoint ScreenResolution = InResolution;
	if (ScreenResolution == FIntPoint::ZeroValue)
	{
		TArray<FIntPoint> Resolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		if (Resolutions.Num())
		{
			ScreenResolution = Resolutions.Last();
		}
	}

	GameUserSettings->SetScreenResolution(ScreenResolution);
	NotifyValueUpdate();
}

FIntPoint UVSSettingItem_ScreenResolution::GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	FIntPoint ScreenResolution = FIntPoint::ZeroValue;
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		ScreenResolution = GameUserSettings->GetDefaultResolution();
		break;
		
	case EVSSettingItemValueSource::Current:
		ScreenResolution =  FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY);
		break;
		
	case EVSSettingItemValueSource::Settings:
		ScreenResolution =  GameUserSettings->GetScreenResolution();
		break;
		
	case EVSSettingItemValueSource::LastConfirmed:
		ScreenResolution =  GameUserSettings->GetLastConfirmedScreenResolution();
		break;
		
	default:
		ScreenResolution = FIntPoint::ZeroValue;
		break;
	}

	if (ScreenResolution == FIntPoint::ZeroValue)
	{
		ScreenResolution = GameUserSettings->GetDesktopResolution();
	}

	return ScreenResolution;
}

FText UVSSettingItem_ScreenResolution::ResolutionToText(const FIntPoint& Resolution) const
{
	const FString XStr = LexToString(Resolution.X);
	const FString YStr = LexToString(Resolution.Y);

	const int32 FieldWidth = FMath::Max(XStr.Len(), YStr.Len());

	FString Left = XStr;
	if (Left.Len() < FieldWidth)
	{
		Left = FString::ChrN(FieldWidth - Left.Len(), TEXT(' ')) + Left;
	}

	FString Right = YStr;
	if (Right.Len() < FieldWidth)
	{
		Right += FString::ChrN(FieldWidth - Right.Len(), TEXT(' '));
	}

	return FText::FromString(Left + MiddleLinkText.ToString() + Right);
}

FIntPoint UVSSettingItem_ScreenResolution::TextToResolution(const FText& Text) const
{
	const FString InStr = Text.ToString();
	const FString Sep   = MiddleLinkText.ToString();

	FString Left, Right;
	if (!InStr.Split(Sep, &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromStart))
	{
		return FIntPoint::ZeroValue;
	}

	Left.TrimStartAndEndInline();
	Right.TrimStartAndEndInline();

	return FIntPoint(FCString::Atoi(*Left), FCString::Atoi(*Right));
}
