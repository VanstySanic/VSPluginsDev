// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovMantleVaultTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_MantleVaultMovement.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.MantleVault")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_MantleVaultMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()


protected:
	virtual void OnFeatureDeactivated_Implementation() override;
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	bool IsMantlingOrVaultingMode() const;

	/**
	 * Try mantle or vault.
	 * If you are controlling rotation using orientation control 2D feature, set the moving and idle evaluate type to Aim.Direction.
	 * @param WallTraceDirection The direction to trace wall. If Left zero, will use the default algorithm.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows, WallTraceDirection, NetExecPolicies"))
	void TryMantleVault(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection = FVector::ZeroVector, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType = EVSMantleVaultMovementType::MantleOrVault, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

protected:
	/** Stop the mantle / vault movement. This is not replicated. */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	void StopMantleVault();

public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSMantleVaultSnappedParams GetMantleVaultSnappedParams() const { return MovementData.SnappedParams; }

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSMantleVaultCachedParams GetMantleVaultCachedParams() const { return MovementData.CachedParams; }


private:
	bool TryMantleVaultInternal(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType) ;
	void MantleVaultBySnappedParams(const FVSMantleVaultSnappedParams& SnappedParams);

	/** Get the first snapped entry that meets with the requirements. */
	bool CalcMantleVaultSnappedParams(FVSMantleVaultSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow, const FVector& WallTraceDirection, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType) const;

	UFUNCTION(Server, Reliable)
	void TryMantleVault_Server(const TArray<FDataTableRowHandle>& SettingRows, const FVector& WallTraceDirection, EVSMantleVaultMovementType::Type SupportedMovementType, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(Server, Reliable)
	void MantleVault_Server(const FVSMantleVaultSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);
	
	UFUNCTION(NetMulticast, Reliable)
	void MantleVault_Multicast(const FVSMantleVaultSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (RowType = "/Script/VSMovementSystem.VSMantleVaultSettings"))
	TArray<FDataTableRowHandle> DefaultSettingRows;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSGameplayTagEventQuery EntranceTagQuery = FVSGameplayTagEventQuery::GetEmptyPass();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSGameplayTagEventQuery AutoBreakTagQuery;
	
	/**
	 * The time when the animation begins to scale its movement to the reach-target location.
	 * If not assigned, use the start time as the scale begin (as most animation requires that).
	 * If you don't want to scale the movement, set the time equal to GroundPivot.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimScaleMovementToReachTargetTimeMarkName = FName("ScaleMovementToReachTarget");

	/** The time when the movement first reaches the target wall / ground. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimReachTargetTimeMarkName = FName("ReachTarget");
	
	/**
	 * The time when the mantle / vault-start movement has solidly reached the ground / found a solid pivot.
	 * Also used as a possible early end for anim that support both mantle and vault.
	 * Player can break the state with input after that time if mantling.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimGroundPivotTimeMarkName = FName("GroundPivot");
	
	/**
	 * The time when the vault movement begins to go off the platform.
	 * Player can break the state with input after that time if vaulting.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimVaultOffPlatformTimeMarkName = FName("VaultOffPlatform");

	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugShapes = false;
#endif

private:
	struct FMovementData
	{
		FVSMantleVaultSnappedParams SnappedParams;
		FVSMantleVaultCachedParams CachedParams;

		FVSMantleVaultSettings* SettingsPtr = nullptr;
		FVSMantleVaultAnimSettings* AnimSettingsPtr = nullptr;
		FVSAnimSequenceReference* AnimPtr = nullptr;

		float MovementElapsedTime = 0.f;
		FVector LastUpdatedRootLocationRS = FVector::ZeroVector;
		float CapsuleHalfHeightOffsetUSCZ = 0.f;
		bool bMatchesEntranceTagQuery = 0.f;
	} MovementData;
};
