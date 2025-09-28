// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemBase_Integer.h"
#include "VSSettingItem_ScalabilityQualityLevel.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Scalability.QualityLevels")
class VSSETTINGSYSTEM_API UVSSettingItem_ScalabilityQualityLevel : public UVSSettingItemBase_Integer
{
	GENERATED_UCLASS_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	
	virtual void SetValue_Implementation(int32 InValue) override;
	virtual int32 GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityLevel(int32 InQualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetQualityLevel(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
#if WITH_EDITOR
	static const TMap<FGameplayTag, FText>& GetDefaultScalabilityQualityLevelNames();
#endif
	
private:
	int32 LastConfirmedQualityLevel = 0;
};
