// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Classes/VSObjectFeature.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Types/VSTreeQueryTypes.h"
#include "VSInputActionAxisBinderFeature.generated.h"

class UInputAction;
USTRUCT(BlueprintType)
struct FVSInputActionBinderSettings
{
	GENERATED_BODY()
	FVSInputActionBinderSettings()
		: bBindByDefault(false)
	{
	}

	bool IsValid() const
	{
		if (ActionName.IsNone()) return false;
		if (InputEvents.IsEmpty()) return false;

		return true;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EInputEvent>> InputEvents;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputChord> SpecifiedKeys;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

USTRUCT(BlueprintType)
struct FVSInputAxisBinderSettings
{
	GENERATED_BODY()
	FVSInputAxisBinderSettings()
		: bBindByDefault(false)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AxisName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKey> SpecifiedKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

USTRUCT(BlueprintType)
struct FVSEnhancedInputActionBinderSettings
{
	GENERATED_BODY()
	FVSEnhancedInputActionBinderSettings()
		: bBindByDefault(false)
	{
	}
	
	bool IsValid() const
	{
		if (ActionName.IsNone()) return false;
		if (!InputAction || TriggerEvents.IsEmpty()) return false;

		return true;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETriggerEvent> TriggerEvents;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

/**
 * 
 */
UCLASS(DisplayName = "VS.Feature.InputBinder.Action")
class VSPLUGINSCORE_API UVSInputActionBinderFeature : public UVSObjectFeature, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FInputActionDelegate, UVSInputActionBinderFeature* /** Feature */, FName /** ActionName */, EInputEvent /** InputEvent */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInputActionEvent, UVSInputActionBinderFeature*, Feature, FName, ActionName, EInputEvent, InputEvent);

public:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.
	
	//~ End IVSGameplayTagFeatureInterface Interface
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const override;
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ Begin IVSGameplayTagFeatureInterface Interface
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	UInputComponent* GetInputComponent() const;
	
private:
	void BindActionSettings(const FVSInputActionBinderSettings& InActionSettings);
	void UnbindActionSettings(const FVSInputActionBinderSettings& InActionSettings);
	void RefreshActions(const FGameplayTagContainer& TagEvents);
	void HandleActionTriggered(FName ActionName, EInputEvent InputEvent);

public:
	FInputActionDelegate OnActionTriggered_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputActionEvent OnActionTriggered;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputActionBinderSettings> ActionSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<UInputComponent> FeatureInputComponent;
	TSet<int32> BoundActionSettings;
	TMultiMap<uint64, FInputChord> ActionKeyMap;
	TMap<uint64, int32> ActionEventAnyKey;
	TSet<uint64> ActionEventBoundKeys;
};


/** ------------------------------------------------------------------------- **/


/**
 * 
 */
UCLASS(DisplayName = "VS.Feature.InputBinder.Axis")
class VSPLUGINSCORE_API UVSInputAxisBinderFeature : public UVSObjectFeature, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FInputAxisDelegate, UVSInputAxisBinderFeature* /** Feature */, FName /** ActionName */, float /** AxisValue */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInputAxisEvent, UVSInputAxisBinderFeature*, Feature, FName, ActionName, float, AxisValue);

public:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.
	
	//~ End IVSGameplayTagFeatureInterface Interface
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const override;
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ Begin IVSGameplayTagFeatureInterface Interface
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	UInputComponent* GetInputComponent() const;
	
private:
	void BindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings);
	void UnbindAxisSettings(const FVSInputAxisBinderSettings& InActionSettings);
	void RefreshAxes(const FGameplayTagContainer& TagEvents);
	void HandleAxisTriggered(FName AxisName, float Value);

public:
	FInputAxisDelegate OnAxisExecuted_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputAxisEvent OnAxisTriggered;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputAxisBinderSettings> AxisSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<UInputComponent> FeatureInputComponent;
	TSet<int32> BoundAxisSettings;
	TMultiMap<FKey, FName> AxisKeyNameMap;
	TSet<FKey> AxisKeyBoundKeys;
	TMultiMap<FName, uint8> AxisNameCounts;
};


/** ------------------------------------------------------------------------- **/


/**
 * 
 */
UCLASS(DisplayName = "VS.Feature.InputBinder.EnhancedAction")
class VSPLUGINSCORE_API UVSEnhancedInputActionBinderFeature : public UVSObjectFeature, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_FourParams(FInputActionDelegate, UVSEnhancedInputActionBinderFeature* /** Feature */, FName /** ActionName */, ETriggerEvent /** TriggerEvent */, FInputActionInstance /** ActionInstance */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInputActionEvent, UVSEnhancedInputActionBinderFeature*, Feature, FName, ActionName, ETriggerEvent, TriggerEvent, FInputActionInstance, ActionInstance);
	
public:
	//~ Begin UVSObjectFeature Interface.
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface.
	
	//~ End IVSGameplayTagFeatureInterface Interface
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const override;
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ Begin IVSGameplayTagFeatureInterface Interface
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	UEnhancedInputComponent* GetEnhancedInputComponent() const;

private:
	void BindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings);
	void UnbindActionSettings(const FVSEnhancedInputActionBinderSettings& InActionSettings);
	void RefreshActions(const FGameplayTagContainer& TagEvents);
	void HandleActionExecution(FName ActionName, ETriggerEvent TriggerEvent, const FInputActionInstance& ActionInstance);

public:
	FInputActionDelegate OnActionTriggered_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputActionEvent OnActionTriggered;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FVSEnhancedInputActionBinderSettings> ActionSettings;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<UEnhancedInputComponent> FeatureInputComponent;
	TSet<int32> BoundEnhancedActionSettings;
	TMultiMap<uint64, FName> EnhancedActionNameCounts;
	TMap<uint64, uint32> EnhancedBindingHandlesByKey;
};
