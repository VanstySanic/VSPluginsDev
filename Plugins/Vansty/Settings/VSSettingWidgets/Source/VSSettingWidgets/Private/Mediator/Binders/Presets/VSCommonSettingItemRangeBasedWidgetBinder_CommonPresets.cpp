// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/Presets/VSCommonSettingItemRangeBasedWidgetBinder_CommonPresets.h"
#include "GameplayTagContainer.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingSystemTags.h"

UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets::UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FVector2D UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets::GenerateValueRange_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::GenerateValueRange_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemIdentifier == EVSSettingItem::Video::FrameRateLimit)
	{
		return FVector2D(0.0, 360.0);
	}
	if (ItemIdentifier == EVSSettingItem::Scalability::ResolutionScale)
	{
		return FVector2D(FMath::Max(Scalability::MinResolutionScale, 10.0), Scalability::MaxResolutionScale);
	}
	if (ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return FVector2D(0.0, 1.25);
	}
	
	return Super::GenerateValueRange_Implementation();
}

#if WITH_EDITOR
bool UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets::EditorAllowChangingStepSize_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingStepSize_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);

	if (ItemIdentifier == EVSSettingItem::Video::FrameRateLimit
		|| ItemIdentifier == EVSSettingItem::Scalability::ResolutionScale
		|| ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingStepSize_Implementation();
}

bool UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets::EditorAllowChangingSnapByStep_Implementation() const
{
	if (!GetSettingItem_Native()) return Super::EditorAllowChangingSnapByStep_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemIdentifier == EVSSettingItem::Video::FrameRateLimit
		|| ItemIdentifier == EVSSettingItem::Scalability::ResolutionScale
		|| ItemIdentifier.MatchesTag(AudioVolumeParentTag))
	{
		return false;
	}
	
	return Super::EditorAllowChangingSnapByStep_Implementation();
}

void UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets::EditorRefreshMediator_Implementation()
{
	Super::EditorRefreshMediator_Implementation();

	if (!GetSettingItem_Native()) return;
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag AudioVolumeParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Volume::Master);
	
	if (ItemIdentifier == EVSSettingItem::Video::FrameRateLimit)
	{
		StepSize = 1.f;
		bSnapByStep = true;
	}
	else if (ItemIdentifier == EVSSettingItem::Scalability::ResolutionScale)
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
