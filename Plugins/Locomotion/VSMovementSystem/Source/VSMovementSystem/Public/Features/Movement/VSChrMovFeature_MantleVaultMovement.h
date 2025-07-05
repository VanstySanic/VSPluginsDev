// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovMantleVaultTypes.h"
#include "VSChrMovFeature_MantleVaultMovement.generated.h"

/**
 * If you are controlling rotation using orientation control 2D feature, set the moving and idle evaluate type to aiming.
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.MantleVault")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_MantleVaultMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	bool IsMantlingOrVaultingMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	void TryMantleVault(const TArray<FDataTableRowHandle>& SettingRows, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType = EVSMantleVaultMovementType::MantleOrVault);

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	void StopMantleVault();
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSMantleVaultSnappedParams GetMantleVaultSnappedParams() const { return MovementData.SnappedParams; }

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "Settings"))
	FVSMantleVaultCachedParams GetMantleVaultCachedParams() const { return MovementData.CachedParams; }

	
protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

private:
	void TryMantleVaultInternal(const TArray<FDataTableRowHandle>& SettingRows, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType);
	void MantleVaultBySnappedParams(const FVSMantleVaultSnappedParams& SnappedParams);

	/** Get the first snapped entry that meets with the requirements. */
	bool CalcMantleVaultSnappedParams(FVSMantleVaultSnappedParams& OutSnappedParams, const FDataTableRowHandle& SettingsRow, TEnumAsByte<EVSMantleVaultMovementType::Type> SupportedMovementType);

	UFUNCTION(Server, Reliable)
	void TryMantleVault_Server(const TArray<FDataTableRowHandle>& SettingRows, EVSMantleVaultMovementType::Type SupportedMovementType);
		
	UFUNCTION()
	void OnRep_ReplicatedSnappedParams();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (RowType = "/Script/VSMovementSystem.VSMantleVaultSettings"))
	TArray<FDataTableRowHandle> DefaultSettingRows;
	
	
	/** The time when the movement first reaches the target wall / ground. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimReachTargetTimeMarkName = FName("ReachTarget");
	
	/**
	 * The time when the mantle / vault start movement has solidly reached the ground / found a solid pivot.
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
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedSnappedParams")
	FVSMantleVaultSnappedParams ReplicatedSnappedParams;
	
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
	} MovementData;
};
