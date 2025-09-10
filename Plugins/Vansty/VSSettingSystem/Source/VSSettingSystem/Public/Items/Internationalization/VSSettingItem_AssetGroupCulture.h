// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemBase_String.h"
#include "VSSettingItem_AssetGroupCulture.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingItem_AssetGroupCulture : public UVSSettingItemBase_String
{
	GENERATED_UCLASS_BODY()
	
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void Initialize_Implementation() override;
	
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;

	virtual void SetValue_Implementation(const FString& InValue) override;
	virtual FString GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const override;

	virtual void OnCultureChanged_Implementation() override;
	virtual TArray<FString> CalcGeneratedOptions_Implementation() const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetAssetGroupCulture(const FString& InCulture);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetAssetGroupCulture(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RefreshNamedCultures();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Culture")
	FName AssetGroup = NAME_None;
	
	UPROPERTY(EditAnywhere, Category = "Culture")
	bool bUseNativeCultureName = true;
	
private:
	FString Culture;
	FString LastConfirmedCulture;
};
