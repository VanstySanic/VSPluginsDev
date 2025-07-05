// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "VSChrMovAnimFeature_LandRecovery.generated.h"

class UVSChrMovFeature_LandRecovery;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMovAnim.Details.LandRecovery", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_LandRecovery : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasValidLandRecoveryAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanEnterNonAdditiveLandRecoveryState() const;

	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool CanLandRecoveryAnimPlayAsAdditive() const;
	
	
	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupLandRecoveryAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateLandRecoveryAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

protected:
	virtual void Initialize_Implementation() override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName AnimReachGroundTimeMarkName = FName("ReachGround");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName AnimEarliestOutWithInputTimeMarkName = FName("EarliestOutWithInput");

	
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_LandRecovery> ChrMovFeature_LandRecovery;

private:
	struct FAnimData
	{
		int32 LastActionID = 0;
	} AnimData;
};
