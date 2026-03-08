// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Classes/VSObjectFeature.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "VSMovementFeatureBase.generated.h"

class UMovementComponent;
class UVSGameplayTagFeatureBase;

/**
 * Base movement data feature shared by movement-system implementations.
 */
UCLASS(Abstract)
class VSMOVEMENTSYSTEMBASE_API UVSMovementFeatureBase : public UVSObjectFeature, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSObjectFeature Interface
	virtual void TickFeature(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction) override;
	//~ End UVSObjectFeature Interface
	
	//~ Begin IVSGameplayTagFeatureInterface
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const override;
	//~ End IVSGameplayTagFeatureInterface

	/** Returns true when this feature should run movement updates during TickFeature. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	bool CanUpdateMovement() const;
	
	/** Performs per-tick movement update logic when CanUpdateMovement returns true. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void UpdateMovement(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementModeChanged(const FGameplayTag& NewMovementMode, const FGameplayTag& PrevMovementMode);
	
public:
	/** Returns current world-space velocity for this movement source. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement|Data")
	FVector GetVelocity() const;

	/** Returns current world-space angular velocity for this movement source. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement|Data")
	FVector GetAngularVelocity() const;
	
	/** Returns current world-space acceleration for this movement source. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	FVector GetAcceleration() const;

	/** Returns movement input in world space. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	FVector GetMovementInput() const;
	
	/** Returns the scene component currently updated by movement simulation. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	USceneComponent* GetUpdatedComponent() const;

	/** Returns the movement base component currently affecting this feature. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	UPrimitiveComponent* GetMovementBase() const;

	/** Returns the movement-base bone name when based on a skeletal component. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	FName GetMovementBaseBoneName() const;
	
	/** Returns current movement mode tag. */
	UFUNCTION(BlueprintNativeEvent, Category = "Movement|Data")
	FGameplayTag GetMovementMode() const;
};
