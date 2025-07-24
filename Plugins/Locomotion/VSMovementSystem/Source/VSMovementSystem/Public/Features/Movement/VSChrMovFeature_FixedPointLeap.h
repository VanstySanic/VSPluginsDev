// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSChrMovInAirTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_FixedPointLeap.generated.h"

struct FVSAnimSequenceReference;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.FixedPointLeap")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_FixedPointLeap : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;

	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	bool IsFixedPointLeapMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows, NetExecPolicies"))
	void TryFixedPointLeap(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocation, USceneComponent* ComponentToFollow = nullptr, const FVSNetMethodExecutionPolicies& NetExecPolicies = FVSNetMethodExecutionPolicies());

protected:
	/** Stop the movement. This is not replicated. */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	void StopFixPointLeap();

public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	FVSFixedPointLeapSnappedParams GetFixedPointLeapSnappedParams() const { return MovementData.SnappedParams; }

	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "SettingRows"))
	FVSFixedPointLeapCachedParams GetFixedPointLeapCachedParams() const { return MovementData.CachedParams; }
	
private:
	bool TryFixedPointLeapInternal(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, USceneComponent* ComponentToFollow);
	void FixedPointLeapBySnappedParams(const FVSFixedPointLeapSnappedParams& SnappedParams);

	UFUNCTION(Server, Reliable)
	void TryFixedPointLeap_Server(const TArray<FDataTableRowHandle>& SettingRows, const FVector& TargetRootLocationUndefinedSpace, USceneComponent* ComponentToFollow, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);

	UFUNCTION(NetMulticast, Reliable)
	void TryFixedPointLeap_Multicast(const FVSFixedPointLeapSnappedParams& SnappedParams, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (RowType = "/Script/VSMovementSystem.VSFixedPointLeapSettings"))
	TArray<FDataTableRowHandle> DefaultSettingRows;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimMovementStartTimeMarkName = FName("MovementStart");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimMovementEndTimeMarkName = FName("MovementEnd");

private:
	struct FMovementData
	{
		FVSFixedPointLeapSnappedParams SnappedParams;
		FVSFixedPointLeapCachedParams CachedParams;
		float MovementElapsedTime = 0.f;
		FVSFixedPointLeapSettings* SettingsPtr = nullptr;
		FVSAnimSequenceReference* AnimPtr = nullptr;
		float CapsuleHalfHeightOffsetUSCZ = 0.f;
	} MovementData;
};
