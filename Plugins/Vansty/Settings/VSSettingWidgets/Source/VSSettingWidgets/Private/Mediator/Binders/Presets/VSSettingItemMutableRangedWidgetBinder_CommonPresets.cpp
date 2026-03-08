// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/Presets/VSSettingItemMutableRangedWidgetBinder_CommonPresets.h"
#include "GameplayTagContainer.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingSystemTags.h"

UVSSettingItemMutableRangedWidgetBinder_CommonPresets::UVSSettingItemMutableRangedWidgetBinder_CommonPresets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FVector2D UVSSettingItemMutableRangedWidgetBinder_CommonPresets::GenerateValueRange_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::GenerateValueRange_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(VS::Settings::Item::Audio::Volume::Master);
	
	if (ItemIdentifier == VS::Settings::Item::Video::FrameRateLimit)
	{
		return FVector2D(0.0, 360.0);
	}
	if (ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return FVector2D(0.0, 1.25);
	}
	
	return Super::GenerateValueRange_Implementation();
}

#if WITH_EDITOR
bool UVSSettingItemMutableRangedWidgetBinder_CommonPresets::EditorAllowChangingStepSize_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingStepSize_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(VS::Settings::Item::Audio::Volume::Master);

	if (ItemIdentifier == VS::Settings::Item::Video::FrameRateLimit
		|| ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingStepSize_Implementation();
}

bool UVSSettingItemMutableRangedWidgetBinder_CommonPresets::EditorAllowChangingSnapByStep_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingSnapByStep_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(VS::Settings::Item::Audio::Volume::Master);

	if (ItemIdentifier == VS::Settings::Item::Video::FrameRateLimit
		|| ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingSnapByStep_Implementation();
}

void UVSSettingItemMutableRangedWidgetBinder_CommonPresets::EditorRefreshMediator_Implementation()
{
	Super::EditorRefreshMediator_Implementation();

	if (!GetSettingItem_Native()) return;
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(VS::Settings::Item::Audio::Volume::Master);
	
	if (ItemIdentifier == VS::Settings::Item::Video::FrameRateLimit)
	{
		StepSize = 1.f;
		bSnapByStep = true;
	}
	else if (ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		StepSize = 0.01f;
		bSnapByStep = true;
	}
}
#endif
