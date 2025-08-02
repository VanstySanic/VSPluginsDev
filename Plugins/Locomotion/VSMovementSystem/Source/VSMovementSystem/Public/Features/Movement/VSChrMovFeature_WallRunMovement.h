// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovWallRunTypes.h"
#include "Types/VSGameplayTypes.h"
#include "Types/Animation/VSAnimSequenceReference.h"
#include "VSChrMovFeature_WallRunMovement.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.WallRun")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_WallRunMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	bool IsWallRunMode() const;
	
	/**
	 * Try start wall run process from ground or in air.
	 * If you are controlling rotation using orientation control 2D feature, set the moving and idle evaluate type to Aim.Direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows, NetExecPolicies"))
	void TryWallRun(const TArray<FDataTableRowHandle>& SettingRows, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());
	
	/**
	 * End the wall run movement.
	 * @param bTryWithEndingMovement Whether to process the ending movement and animation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "NetExecPolicies"))
	void EndWallRun(bool bTryWithEndingMovement, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

	/**
	 * Suggest a launch velocity that allows user to start a new wall run process on the other side wall, without changing the Z axis location.
	 * This only works when the character is on the wall, and the other side wall should be paralleled with the current.
	 * @WallDistanceRange The distance range between the current wall and other side wall.
	 * @SettingRow If null, will get from current settings.
	 * @MovementDuration The movement duration in air, from the current wall to other side wall.
	 * @TraceDistanceToWall Character will enter wall run movement within the distance to the other side wall.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "WallDistanceRange, SettingRow"))
	bool SuggestLaunchVelocityToOtherSideParalleledWall(FVector& OutVelocity, const FVector2D& WallDistanceRange = FVector2D(256.0, 512.0), const float MovementDuration = 0.67f, const FDataTableRowHandle& SettingRow = FDataTableRowHandle()) const;

	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSWallRunSnappedParams GetWallRunSnappedParams() const { return MovementData.SnappedParams; }

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSWallRunCachedParams GetWallRunCachedParams() const { return MovementData.CachedParams; }

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FGameplayTag GetWallRunState() const { return MovementData.WallRunState; }

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "NewWallRunState"))
	void SetWallRunState(const FGameplayTag& NewWallRunState);
	
private:
	bool TryWallRunInternal(const TArray<FDataTableRowHandle>& SettingRows);
	void EndWallRunInternal(bool bTryEndMovement);
	void WallRunBySnappedParams(const FVSWallRunSnappedParams& SnappedParams);
	
	/** Get the first snapped entry that meets with the requirements. */
	bool CalcWallRunSnappedParams(FVSWallRunSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow) const;

	
	UFUNCTION(Server, Reliable)
	void TryWallRun_Server(const TArray<FDataTableRowHandle>& SettingRows, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Server, Reliable)
	void WallRun_Server(const FVSWallRunSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(NetMulticast, Reliable)
	void WallRun_Multicast(const FVSWallRunSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Server, Reliable)
	void EndWallRun_Server(bool bTryEndMovement, EVSNetAuthorityMethodExecPolicy::Type NetPolicy);

	UFUNCTION(NetMulticast, Reliable)
	void EndWallRun_Multicast(bool bTryEndMovement, EVSNetAuthorityMethodExecPolicy::Type NetPolicy);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (RowType = "/Script/VSMovementSystem.VSWallRunSettings"))
	TArray<FDataTableRowHandle> DefaultSettingRows;

	
	/** This is the time when start-from-ground anim reaches the wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimStartReachWallMarkName = FName("ReachWall");
	
	/** This is the time when end anim leaves the wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimEndLeaveWallMarkName = FName("LeaveWall");
    	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugShapes = false;
#endif

private:
	struct FMovementData
	{
		FVSWallRunSnappedParams SnappedParams;
		FVSWallRunCachedParams CachedParams;

		FVSWallRunSettings* SettingsPtr = nullptr;
		FVSAnimSequenceReference* StartAnimPtr = nullptr;
		FVSAnimSequenceReference* EndAnimPtr = nullptr;
		
		FVector LastUpdatedRootLocationRS = FVector::ZeroVector;
		FVector LastTracedWallNormalRS = FVector::ZeroVector;
		FVector LastTracedWallPointRS = FVector::ZeroVector;
		FVector LastUpdatedStartMovementCurveValues = FVector::ZeroVector;
		FVector LastUpdatedEndMovementCurveValues = FVector::ZeroVector;
		FGameplayTag WallRunState = FGameplayTag::EmptyTag;
		float StartMovementElapsedTime = 0.f;
		float EndMovementElapsedTime = 0.f;

		float CapsuleHalfHeightOffsetUSCZ = 0.f;
	} MovementData;
};
