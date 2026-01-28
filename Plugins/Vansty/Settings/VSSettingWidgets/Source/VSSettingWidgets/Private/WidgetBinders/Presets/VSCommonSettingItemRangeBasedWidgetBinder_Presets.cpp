// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/Presets/VSCommonSettingItemRangeBasedWidgetBinder_Presets.h"
#include "GameplayTagContainer.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingSystemTags.h"

UVSCommonSettingItemRangeBasedWidgetBinder_Presets::UVSCommonSettingItemRangeBasedWidgetBinder_Presets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FVector2D UVSCommonSettingItemRangeBasedWidgetBinder_Presets::GenerateValueRange_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::GenerateValueRange_Implementation();
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemTag == EVSSettingItem::Video::FrameRateLimit)
	{
		return FVector2D(0.0, 360.0);
	}
	if (ItemTag == EVSSettingItem::Scalability::ResolutionScale)
	{
		return FVector2D(Scalability::MinResolutionScale, Scalability::MaxResolutionScale);
	}
	if (ItemTag.MatchesTag(AudioVolumeParentTag))
	{
		return FVector2D(0.0, 1.25);
	}
	
	return Super::GenerateValueRange_Implementation();
}

#if WITH_EDITOR
bool UVSCommonSettingItemRangeBasedWidgetBinder_Presets::EditorAllowChangingStepSize_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingStepSize_Implementation();
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);

	if (ItemTag == EVSSettingItem::Video::FrameRateLimit
		|| ItemTag == EVSSettingItem::Scalability::ResolutionScale
		|| ItemTag.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingStepSize_Implementation();
}

bool UVSCommonSettingItemRangeBasedWidgetBinder_Presets::EditorAllowChangingSnapByStep_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingSnapByStep_Implementation();
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemTag == EVSSettingItem::Video::FrameRateLimit
		|| ItemTag == EVSSettingItem::Scalability::ResolutionScale
		|| ItemTag.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingSnapByStep_Implementation();
}

void UVSCommonSettingItemRangeBasedWidgetBinder_Presets::EditorRefreshMediator_Implementation()
{
	Super::EditorRefreshMediator_Implementation();

	if (!GetSettingItem_Native()) return;
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemTag == EVSSettingItem::Video::FrameRateLimit)
	{
		StepSize = 1.f;
		bSnapByStep = true;
	}
	else if (ItemTag == EVSSettingItem::Scalability::ResolutionScale)
	{
		StepSize = 1.f;
		bSnapByStep = true;
	}
	else if (ItemTag.MatchesTag(AudioVolumeParentTag))
	{
		StepSize = 0.01f;
		bSnapByStep = true;
	}
}
#endif
