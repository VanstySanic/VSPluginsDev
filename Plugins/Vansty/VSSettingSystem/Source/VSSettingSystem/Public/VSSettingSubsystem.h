// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingSubsystem.generated.h"

class UVSSettingItemSet;

/**
 * 
 */
UCLASS()
class VSSETTINGSYSTEM_API UVSSettingSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemUpdateSignature, UVSSettingItemBase*, SettingItem);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSettingItemActionSignature, UVSSettingItemBase*, SettingItem, EVSSettingItemAction::Type, Action);

public:
	UVSSettingSubsystem();
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	static UVSSettingSubsystem* Get();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<UVSSettingItemBase*> GetSettingItems() const;

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateSignature OnSettingItemUpdated;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemActionSignature OnSettingItemActionExecuted;

private:
	UPROPERTY()
	TArray<TObjectPtr<UVSSettingItemSet>> SettingItemSets;
	TArray<TWeakObjectPtr<UVSSettingItemBase>> SettingItems;
};
