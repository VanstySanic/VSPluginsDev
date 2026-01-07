// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/Graphics/VSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel.h"

UVSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel::UVSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

TMap<FString, FText> UVSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel::GetOptionKeyedTexts_Implementation() const
{
	static TMap<FString, FText> StaticOptionKeyedTexts = TMap<FString, FText>
	{
		{ "0", Scalability::GetQualityLevelText(0, 5) },
		{ "1", Scalability::GetQualityLevelText(1, 5) },
		{ "2", Scalability::GetQualityLevelText(2, 5) },
		{ "3", Scalability::GetQualityLevelText(3, 5) },
		{ "4", Scalability::GetQualityLevelText(4, 5) },
	};
	
	return StaticOptionKeyedTexts;
}
