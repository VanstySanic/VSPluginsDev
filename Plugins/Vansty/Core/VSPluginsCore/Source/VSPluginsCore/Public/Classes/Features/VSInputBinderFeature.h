// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Classes/VSObjectFeature.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Types/VSGameQueryTypes.h"
#include "VSInputBinderFeature.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FVSInputActionBinderSettings
{
	GENERATED_BODY()
	FVSInputActionBinderSettings()
		: bBindByDefault(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EInputEvent>> InputEvents;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey SpecifiedKey = FKey();
	
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
	FKey SpecifiedKey = FKey();

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ETriggerEvent> TriggerEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;
	
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
UCLASS()
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

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputActionBinderSettings> ActionSettings;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<AController> ControllerPrivate;
};


/** ------------------------------------------------------------------------- **/


/**
 * 
 */
UCLASS()
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
	void RefreshAxis(const FGameplayTagContainer& TagEvents);
	void HandleAxisTriggered(FName AxisName, float Value);

public:
	FInputAxisDelegate OnAxisExecuted_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FInputAxisEvent OnAxisTriggered;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TArray<FVSInputAxisBinderSettings> AxisSettings;
	
private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<AController> ControllerPrivate;
};


/** ------------------------------------------------------------------------- **/


/**
 * 
 */
UCLASS()
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

private:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<AController> ControllerPrivate;
};