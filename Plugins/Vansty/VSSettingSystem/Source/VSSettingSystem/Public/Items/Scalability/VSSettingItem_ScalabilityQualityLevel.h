// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_ScalabilityQualityLevel.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Scalability.QualityLevels")
class VSSETTINGSYSTEM_API UVSSettingItem_ScalabilityQualityLevel : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityLevel(int32 InQualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
#if WITH_EDITOR
	static const TMap<FGameplayTag, FText>& GetDefaultScalabilityQualityLevelNames();
#endif


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TMap<int32, FText> QualityLevelNames;
	
private:
	int32 LastConfirmedQualityLevel = 0;
};
