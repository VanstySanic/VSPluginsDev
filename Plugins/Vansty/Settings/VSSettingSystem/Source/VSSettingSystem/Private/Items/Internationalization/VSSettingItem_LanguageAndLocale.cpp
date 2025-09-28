// Copyright VanstySanic. All Rights Reserved.

#include "Items/Internationalization/VSSettingItem_LanguageAndLocale.h"
#include "Internationalization/Culture.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_LanguageAndLocale::UVSSettingItem_LanguageAndLocale(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Internationalization_LanguageAndLocale;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.Internationalization.LanguageAndLocale", "Language");

	ValueUpdatedActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Validate,
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
		EVSSettingItemAction::Save
	};

	NamedValues.Empty();
	const TArray<FString>& AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(
		ELocalizationLoadFlags::Native
		| ELocalizationLoadFlags::Editor
		| ELocalizationLoadFlags::Game
		| ELocalizationLoadFlags::Engine
		| ELocalizationLoadFlags::Additional
		| ELocalizationLoadFlags::ForceLocalizedGame
		| ELocalizationLoadFlags::SkipExisting);
	
	for (const FString& AvailableCulture : AvailableCultures)
	{
		FCulturePtr CulturePtr = FInternationalization::Get().GetCulture(AvailableCulture);
		NamedValues.Emplace(AvailableCulture, FText::FromString(bUseNativeCultureName ? CulturePtr->GetNativeName() : CulturePtr->GetDisplayName()));
	}
	
	RefreshNamedCultures();
}

#if WITH_EDITOR
void UVSSettingItem_LanguageAndLocale::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_LanguageAndLocale, bUseNativeCultureName))
	{
		RefreshNamedCultures();
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_LanguageAndLocale::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	RefreshNamedCultures();
}

void UVSSettingItem_LanguageAndLocale::Load_Implementation()
{
	FString LoadCulture;
	if (GConfig->GetString(TEXT("Internationalization"), TEXT("Language"), LoadCulture, GGameUserSettingsIni))
	{
		SetLanguageAndLocale(LoadCulture);
	}
}

void UVSSettingItem_LanguageAndLocale::Apply_Implementation()
{
	FInternationalization::Get().SetCurrentLanguageAndLocale(Culture);
}

void UVSSettingItem_LanguageAndLocale::Confirm_Implementation()
{
	LastConfirmedCulture = GetLanguageAndLocale(EVSSettingItemValueSource::Settings);
}

void UVSSettingItem_LanguageAndLocale::Save_Implementation()
{
	GConfig->SetString(TEXT("Internationalization"), TEXT("Language"), *Culture, GGameUserSettingsIni);
	GConfig->SetString(TEXT("Internationalization"), TEXT("Locale"), *Culture, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_LanguageAndLocale::SetValue_Implementation(const FString& InValue)
{
	SetLanguageAndLocale(InValue);
}

FString UVSSettingItem_LanguageAndLocale::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetLanguageAndLocale(ValueSource);
}

void UVSSettingItem_LanguageAndLocale::OnCultureChanged_Implementation()
{
	Super::OnCultureChanged_Implementation();
	RefreshNamedCultures();
}

TArray<FString> UVSSettingItem_LanguageAndLocale::CalcGeneratedOptions_Implementation() const
{
	const TArray<FString>& AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(
		UVSGameplayLibrary::IsInGame() ? ELocalizationLoadFlags::Game :
			ELocalizationLoadFlags::Native
			| ELocalizationLoadFlags::Editor
			| ELocalizationLoadFlags::Game
			| ELocalizationLoadFlags::Engine
			| ELocalizationLoadFlags::Additional
			| ELocalizationLoadFlags::ForceLocalizedGame
			| ELocalizationLoadFlags::SkipExisting);
	return AvailableCultures;
}

void UVSSettingItem_LanguageAndLocale::SetLanguageAndLocale(const FString& InCulture)
{
	if (Culture == InCulture) return;
	Culture = InCulture;
	NotifyValueUpdate();
}

FString UVSSettingItem_LanguageAndLocale::GetLanguageAndLocale(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return FInternationalization::Get().GetDefaultCulture()->GetName();
		
	case EVSSettingItemValueSource::Current:
		return FInternationalization::Get().GetCurrentCulture()->GetName();

	case EVSSettingItemValueSource::Settings:
		return Culture;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedCulture;

	default:
		break;
	}

	return FInternationalization::Get().GetCurrentCulture()->GetName();
}

void UVSSettingItem_LanguageAndLocale::RefreshNamedCultures()
{
	const TArray<FString>& AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(
		ELocalizationLoadFlags::Native
		| ELocalizationLoadFlags::Editor
		| ELocalizationLoadFlags::Game
		| ELocalizationLoadFlags::Engine
		| ELocalizationLoadFlags::Additional
		| ELocalizationLoadFlags::ForceLocalizedGame
		| ELocalizationLoadFlags::SkipExisting);
	
	for (const FString& AvailableCulture : AvailableCultures)
	{
		if (NamedValues.Contains(AvailableCulture))
		{
			FCulturePtr CulturePtr = FInternationalization::Get().GetCulture(AvailableCulture);
			NamedValues.Emplace(AvailableCulture, FText::FromString(bUseNativeCultureName ? CulturePtr->GetNativeName() : CulturePtr->GetDisplayName()));
		}
	}

	RebindWidgetsOfType(FName("Value"));
	RebindWidgetsOfType(FName("Content"));
}
