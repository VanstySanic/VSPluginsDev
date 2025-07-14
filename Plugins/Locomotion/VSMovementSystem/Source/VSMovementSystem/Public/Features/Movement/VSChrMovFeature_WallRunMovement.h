// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovWallRunTypes.h"
#include "VSChrMovFeature_WallRunMovement.generated.h"

struct FVSAnimSequenceReference;
/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.WallRun")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_WallRunMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	void TryWallRun(const TArray<FDataTableRowHandle>& SettingRows);
	
	/**
	 * End the wall run movement.
	 * @param bTryEndMovement Whether to process the ending movement and animation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void EndWallRun(bool bTryEndMovement, bool bReplicated = false);


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
	void TryWallRunInternal(const TArray<FDataTableRowHandle>& SettingRows);
	void EndWallRunInternal(bool bTryEndMovement);
	void WallRunBySnappedParams(const FVSWallRunSnappedParams& SnappedParams);
	
	/** Get the first snapped entry that meets with the requirements. */
	bool CalcWallRunSnappedParams(FVSWallRunSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow) const;

	
	UFUNCTION(Server, Reliable)
	void TryWallRun_Server(const TArray<FDataTableRowHandle>& SettingRows);

	UFUNCTION(Server, Reliable)
	void EndWallRun_Server(bool bTryEndMovement);

	UFUNCTION(NetMulticast, Reliable)
	void EndWallRun_Multicast(bool bTryEndMovement);
	
	UFUNCTION()
	void OnRep_ReplicatedSnappedParams();


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
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedSnappedParams")
	FVSWallRunSnappedParams ReplicatedSnappedParams;

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
		FVector LastUpdatedStartMovementCurveValues = FVector::ZeroVector;
		FVector LastUpdatedEndMovementCurveValues = FVector::ZeroVector;
		FGameplayTag WallRunState = FGameplayTag::EmptyTag;
		float StartMovementElapsedTime = 0.f;
		float EndMovementElapsedTime = 0.f;
	} MovementData;
};
