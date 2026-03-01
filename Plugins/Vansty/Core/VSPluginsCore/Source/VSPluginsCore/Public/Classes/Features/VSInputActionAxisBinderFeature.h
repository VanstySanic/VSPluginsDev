// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Classes/VSObjectFeature.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Types/VSTreeQueryTypes.h"
#include "VSInputActionAxisBinderFeature.generated.h"

class UInputAction;

/** Binding rules for a classic input action entry. */
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

	/** Action name used for InputComponent action bindings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
	/** Input events to bind for ActionName. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EInputEvent>> InputEvents;
	
	/** Optional key filter. Empty means allow any key mapped to ActionName. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInputChord> SpecifiedKeys;
	
	/** Auto-bind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	/** Auto-unbind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	/** If true, bind this entry on BeginPlay before query refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

/** Binding rules for a classic input axis entry. */
USTRUCT(BlueprintType)
struct FVSInputAxisBinderSettings
{
	GENERATED_BODY()
	FVSInputAxisBinderSettings()
		: bBindByDefault(false)
	{
	}
	
	/** Axis name used for InputComponent axis bindings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AxisName = NAME_None;

	/** Optional key list. Empty means bind by AxisName directly. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKey> SpecifiedKeys;

	/** Auto-bind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	/** Auto-unbind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	/** If true, bind this entry by default before query refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

/** Binding rules for an Enhanced Input action entry. */
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
	
	/** Logical action name used when broadcasting feature delegates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
	/** Enhanced Input action asset to bind. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;

	/** Trigger events to bind for InputAction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETriggerEvent> TriggerEvents;
	
	/** Auto-bind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoBindTagQuery;

	/** Auto-unbind condition evaluated from gameplay tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;

	/** If true, bind this entry by default before query refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bBindByDefault : 1;
};

/**
 * Gameplay-tag-driven binder for legacy InputComponent action bindings.
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
	/** Broadcast when a configured action entry is triggered. */
	FInputActionDelegate OnActionTriggered_Native;

	/** Broadcast when a configured action entry is triggered. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputActionEvent OnActionTriggered;

public:
	/** Action binding entries evaluated and managed by this feature. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputActionBinderSettings> ActionSettings;

	/** Priority used by the internally created InputComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	/** Whether the internally created InputComponent blocks lower-priority input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	/** Input component owned and pushed by this feature. */
	TWeakObjectPtr<UInputComponent> FeatureInputComponent;
	/** Indices currently bound from ActionSettings. */
	TSet<int32> BoundActionSettings;
	/** Per action-event key: configured key filters. */
	TMultiMap<uint64, FInputChord> ActionKeyMap;
	/** Per action-event key: number of "any key" consumers. */
	TMap<uint64, int32> ActionEventAnyKey;
	/** Action-event keys that currently have an InputComponent binding. */
	TSet<uint64> ActionEventBoundKeys;
};


/** ------------------------------------------------------------------------- **/


/**
 * Gameplay-tag-driven binder for legacy InputComponent axis bindings.
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
	/** Broadcast when a configured axis entry is triggered. */
	FInputAxisDelegate OnAxisExecuted_Native;

	/** Broadcast when a configured axis entry is triggered. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputAxisEvent OnAxisTriggered;

public:
	/** Axis binding entries evaluated and managed by this feature. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputAxisBinderSettings> AxisSettings;

	/** Priority used by the internally created InputComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	/** Whether the internally created InputComponent blocks lower-priority input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	/** Input component owned and pushed by this feature. */
	TWeakObjectPtr<UInputComponent> FeatureInputComponent;
	/** Indices currently bound from AxisSettings. */
	TSet<int32> BoundAxisSettings;
	/** Per key: mapped axis names for key-based axis binding mode. */
	TMultiMap<FKey, FName> AxisKeyNameMap;
	/** Keys that currently have a live AxisKeyBinding entry. */
	TSet<FKey> AxisKeyBoundKeys;
	/** AxisName reference counts for name-based axis binding mode. */
	TMultiMap<FName, uint8> AxisNameCounts;
};


/** ------------------------------------------------------------------------- **/


/**
 * Gameplay-tag-driven binder for EnhancedInputComponent action bindings.
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
	/** Broadcast when a configured enhanced action entry is triggered. */
	FInputActionDelegate OnActionTriggered_Native;

	/** Broadcast when a configured enhanced action entry is triggered. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputActionEvent OnActionTriggered;
	
protected:
	/** Enhanced action binding entries evaluated and managed by this feature. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FVSEnhancedInputActionBinderSettings> ActionSettings;
	
public:
	/** Priority used by the internally created EnhancedInputComponent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 InputPriority = 0;

	/** Whether the internally created InputComponent blocks lower-priority input. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 bBlockInput : 1;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	/** Enhanced input component owned and pushed by this feature. */
	TWeakObjectPtr<UEnhancedInputComponent> FeatureInputComponent;
	/** Indices currently bound from ActionSettings. */
	TSet<int32> BoundEnhancedActionSettings;
	/** Per enhanced binding key: logical action names to broadcast. */
	TMultiMap<uint64, FName> EnhancedActionNameCounts;
	/** Per enhanced binding key: live EnhancedInput binding handle. */
	TMap<uint64, uint32> EnhancedBindingHandlesByKey;
};
