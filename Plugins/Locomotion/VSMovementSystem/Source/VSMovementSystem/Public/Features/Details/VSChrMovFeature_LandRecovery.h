// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovGroundedTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_LandRecovery.generated.h"

struct FVSAnimSequenceReference;
class UVSChrMovFeature_WalkingMovement;

/**
 * Land recovery transition for walking mode.
 */
UCLASS(DisplayName = "Feature.ChrMov.Details.LandRecovery")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_LandRecovery : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsLandRecovering() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVSLandRecoveryAnimParams GetLandRecoveryAnimParams() const { return MovementData.AnimParams; }
	
protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

private:
	void CheckLandRecovery();
	void StopLandRecoveryInternal();

	UFUNCTION()
	void OnRep_ReplicatedSettingsRow();
	
protected:
	/** Used when no valid settings row is assigned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land Recovery", meta = (RowType = "/Script/VSLocomotionSystem.VSLandRecoverySettings"))
	FVSDataTableRowHandleWrap DefaultSettingRows;

	/**
	 * Every reassigned row will only use once, until the character reaches ground.
	 * Notice that reassignment will only occur when character enters walking mode.
	 */
	UPROPERTY(EditAnywhere, Category = "Land Recovery", meta = (ShowOnlyInnerProperties, RowType = "/Script/VSLocomotionSystem.VSLandRecoverySettings"))
	TMap<FVSDataTableRowHandleWrap, FGameplayTagQuery> QueriedSettingRows;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bEarlyOutWithInputWhenAdditive = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName AnimReachGroundTimeMarkName = FName("ReachGround");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName AnimEarliestOutWithInputTimeMarkName = FName("EarliestOutWithInput");
	
private:
	struct FMovementData
	{
		float AnimPlayedTime = 0.f;
		FVector LastUpdatedAnimMovementCurves = FVector::ZeroVector;

		FVSLandRecoverySettings* Settings = nullptr;
		FVSAnimSequenceReference* Anim = nullptr;

		FVSLandRecoveryAnimParams AnimParams;
	} MovementData;
};
