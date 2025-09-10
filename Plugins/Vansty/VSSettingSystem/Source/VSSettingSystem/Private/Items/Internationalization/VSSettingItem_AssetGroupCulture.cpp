// Copyright VanstySanic. All Rights Reserved.

#include "Items/Internationalization/VSSettingItem_AssetGroupCulture.h"
#include "Internationalization/Culture.h"

UVSSettingItem_AssetGroupCulture::UVSSettingItem_AssetGroupCulture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
void UVSSettingItem_AssetGroupCulture::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_AssetGroupCulture, bUseNativeCultureName))
	{
		RefreshNamedCultures();
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSSettingItem_AssetGroupCulture::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	RefreshNamedCultures();
}

void UVSSettingItem_AssetGroupCulture::Load_Implementation()
{
	if (const FConfigSection* AssetGroupCulturesSection = GConfig->GetSection(TEXT("Internationalization.AssetGroupCultures"), false, GGameUserSettingsIni))
	{
		if (AssetGroupCulturesSection->Contains(AssetGroup))
		{
			Culture = AssetGroupCulturesSection->FindRef(AssetGroup).GetValue();
		}
	}
}

void UVSSettingItem_AssetGroupCulture::Apply_Implementation()
{
	FInternationalization::Get().SetCurrentAssetGroupCulture(AssetGroup, Culture);
}

void UVSSettingItem_AssetGroupCulture::Confirm_Implementation()
{
	LastConfirmedCulture = GetAssetGroupCulture(EVSSettingItemValueSource::Settings);
}

void UVSSettingItem_AssetGroupCulture::Save_Implementation()
{
	GConfig->RemoveKeyFromSection(TEXT("Internationalization.AssetGroupCultures"), AssetGroup, GGameUserSettingsIni);
	GConfig->AddToSection(TEXT("Internationalization.AssetGroupCultures"), AssetGroup, Culture, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_AssetGroupCulture::SetValue_Implementation(const FString& InValue)
{
	SetAssetGroupCulture(InValue);
}

FString UVSSettingItem_AssetGroupCulture::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetAssetGroupCulture(ValueSource);
}

void UVSSettingItem_AssetGroupCulture::OnCultureChanged_Implementation()
{
	Super::OnCultureChanged_Implementation();
	RefreshNamedCultures();
}

TArray<FString> UVSSettingItem_AssetGroupCulture::CalcGeneratedOptions_Implementation() const
{
	const TArray<FString>& AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(
		ELocalizationLoadFlags::Native
		| ELocalizationLoadFlags::Game
		| ELocalizationLoadFlags::Engine
		| ELocalizationLoadFlags::Additional);
	return AvailableCultures;
}

void UVSSettingItem_AssetGroupCulture::SetAssetGroupCulture(const FString& InCulture)
{
	if (Culture == InCulture) return;
	Culture = InCulture;
	NotifyValueUpdate();
}

FString UVSSettingItem_AssetGroupCulture::GetAssetGroupCulture(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return FInternationalization::Get().GetDefaultCulture()->GetName();
		
	case EVSSettingItemValueSource::Current:
		return FInternationalization::Get().GetCurrentAssetGroupCulture(AssetGroup)->GetName();

	case EVSSettingItemValueSource::Settings:
		return Culture;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedCulture;

	default:
		break;
	}

	return FInternationalization::Get().GetCurrentCulture()->GetName();
}

void UVSSettingItem_AssetGroupCulture::RefreshNamedCultures()
{
	const TArray<FString>& AvailableCultures = FTextLocalizationManager::Get().GetLocalizedCultureNames(
		ELocalizationLoadFlags::Native
		| ELocalizationLoadFlags::Game
		| ELocalizationLoadFlags::Engine
		| ELocalizationLoadFlags::Additional);
	
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