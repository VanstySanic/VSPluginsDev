// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovOrientationTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_TurnInPlace2D.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Orientation.TurnInPlace2D")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_TurnInPlace2D : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "TurnInPlace")
	bool IsTurningInPlace() const;

	UFUNCTION(BlueprintCallable, Category = "TurnInPlace")
	FVSTurnInPlaceSnappedParams2D GetTurnInPlaceSnappedParams() const { return MovementData.SnappedParams; }


private:
	void TurnInPlaceCheck(float DeltaTime);
	void StopTurnInPlaceInternal();
	void UpdateMovementTagQueryStates(const FGameplayTag& TagEvent);

	UFUNCTION()
	void OnRep_ReplicatedSnappedParams();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	FGameplayTag OrientationEvaluateType = EVSOrientationEvaluateType::Control;
	
	// /** If true, the trigger delayed time will be declined instead of being reset to zero in some conditions. */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurnInPlace")
	// bool bDeclineTriggerDelayTime = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta = (RowType = "/Script/VSMovementSystem.VSTurnInPlaceSettings2D"))
	FDataTableRowHandle DefaultSettingsRow;

	/** Auto selected by tags, if not overriden. */
	UPROPERTY(EditAnywhere, Category = "TurnInPlace", meta = (ShowOnlyInnerProperties, RowType = "/Script/VSMovementSystem.VSTurnInPlaceSettings2D"))
	TMap<FVSDataTableRowHandleWrap, FVSGameplayTagEventQueryContainer> QueriedSettingRows;
	
	UPROPERTY(EditAnywhere, Category = "TurnInPlace")
	FVSGameplayTagEventQueryContainer RefreshSettingsRowQueries;
	
	/** Turn in place can be checked and processed only if the movement states match the query. */
	UPROPERTY(EditAnywhere, Category = "TurnInPlace")
	FVSGameplayTagEventQueryContainer EntranceTagQueries;

	/** Auto breaks when movement tags matches this. */
	UPROPERTY(EditAnywhere, Category = "TurnInPlace")
	FVSGameplayTagEventQueryContainer AutoBreakTagQueries;

	/**
	 * The turn in place check will be blocked for some time when query matches.
	 * Notice that this will only block the check process but will not break current movement.
	 * Always override the time instead of adding.
	 */
	UPROPERTY(EditAnywhere, Category = "TurnInPlace")
	TMap<float, FVSGameplayTagEventQueryContainer> QueriedCheckBlockTimes;

	
	/** Anim time after that means the turn in place process enters the recovery state and can do another check. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName AnimRecoveryTimeMarkName = FName("Recovery");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName AnimRotationYawCurveName = FName("RotationYaw");


private:
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedSnappedParams")
	FVSTurnInPlaceSnappedParams2D ReplicatedSnappedParams;
	
private:
	struct FMovementData
	{
		FVSDataTableRowHandleWrap CurrentSettingsRow;
		bool bMatchesEntranceTagQuery = false;

		FVSTurnInPlaceSnappedParams2D SnappedParams;
		struct FMovementCachedParams
		{
			float CheckBlockRemainedTime = 0.f;
			float TriggerDelayedTime = 0.f;
			float AnimRotationAngle = 0.f;
			float AnimPlayedTime = 0.f;
			float LastUpdatedAnimRotationYawCurveValue = 0.f;
		} CachedParams;
	} MovementData;
};
