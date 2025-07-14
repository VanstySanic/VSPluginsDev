// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovAnimFeature_FallingMovement.generated.h"

struct FVSFallingMovementAnimSettings;
struct FVSFallingMovementAnimSet;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMovAnim.Movement.Falling", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_FallingMovement : public UVSCharacterMovementAnimFeature
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateAnimation_Implementation(float DeltaTime) override;
	virtual void UpdateAnimationThreadSafe_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasPossibleStartAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasPossibleStartLoopAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasPossibleApexAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasPossibleLandAnim() const;

	
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanExitStartStateNaturally() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanEnterLandStateNaturally() const;

	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	float CalcOrientationWarpingAngle() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector CalcOrientationWarpingDirection() const;

	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupStartLoopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateStartLoopAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupApexAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateApexAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateFallAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateLandAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

private:
	void UpdateMovementTagQueryStates(const FGameplayTag& TagEvent);
	
protected:
	/** Used when no valid anim settings row is renewed. */
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (RowType = "/Script/VSMovementAnim.VSFallingMovementAnimSettings"))
	FDataTableRowHandle DefaultSettingsRow;
	
	UPROPERTY(EditAnywhere, Category = "Animation", meta = (RowType = "/Script/VSMovementAnim.VSFallingMovementAnimSettings"))
	TArray<FDataTableRowHandle> AnimSettingRows;
	
	/**
	 * Reassign the settings when tags matches the container.
	 * Every renewed row will only use once, until the process is over and character reaches ground.
	 * This is designed for reassignment inside falling mode, and doesn't work in other modes.
	 * Notice that reassignment will only occur in falling mode.
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FVSGameplayTagEventQueryContainer ReassignAnimSettingsQuery;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFallingDown = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsMovingUp = false;
	
	/** The distance to land. Scaled. */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DistanceToLandScaled = 65536.f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector OrientationWarpingDirection;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float OrientationWarpingAngle = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bMovementReassignedThisFrame = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName StartAnimLeaveGroundTimeMarkName = FName("LeaveGround");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName StartAnimLoopSwingTimePeriodName = FName("LoopSwing");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName StartAnimApexTimeMarkName = FName("Apex");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName LandAnimReachGroundTimeMarkName = FName("ReachGround");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName DistanceToLandCurveName = FName("Distance");
	
private:
	struct FAnimData
	{
		uint8 AnimSettingsReassignedMark;
		FVSFallingMovementAnimSettings* CurrentAnimSettingsPtr;

		float StartAnimPlayedTime = 0.f;
		float DistanceToLandThreshold = 0.f;
	} AnimData;
};
