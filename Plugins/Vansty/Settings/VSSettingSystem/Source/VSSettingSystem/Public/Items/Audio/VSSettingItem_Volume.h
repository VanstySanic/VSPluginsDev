// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSMutableFloatSettingItem.h"
#include "VSSettingItem_Volume.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Audio.Volume")
class VSSETTINGSYSTEM_API UVSSettingItem_Volume : public UVSMutableFloatSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
	
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual float GetNonMutedValue_Implementation(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const override;
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetVolume(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetNonMutedVolume(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;
	
private:
#if WITH_EDITOR
	static TMap<FGameplayTag, FText> GetVolumeTypeDisplayNames();
	static TMap<FGameplayTag, FString> GetVolumeTypeConfigKeyNames();
#endif

protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	TObjectPtr<USoundMix> SoundMix;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TObjectPtr<USoundClass> SoundClass;
};