// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/VSCharacterMovementTypes.h"
#include "Types/VSChrMovOrientationTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovAnimFeature_AimOffset.generated.h"

class UAimOffsetBlendSpace;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMovAnim.Details.AimOffset", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_AimOffset : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()

	/** Get current aim offset blend space. This is optionally used. */
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	UAimOffsetBlendSpace* GetAimOffsetBlendSpace() const { return AnimData.CurrentAimOffsetBlendSpace; }
	
	/** [Yaw, Pitch] */
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	FVector2D GetAimOffsetAngles() const { return AnimData.AimOffsetAngle; }

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateAnimationThreadSafe_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

private:
	void UpdateTagQueryStates(const FGameplayTag& TagEvent);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FVSMovementOrientationEvaluateType DefaultOrientationEvaluateType = FVSMovementOrientationEvaluateType(EVSMovementRelatedOrientationType::Control);
	
	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FVSMovementOrientationEvaluateType, FVSGameplayTagEventQuery> TagQueriedOrientationEvaluateTypes;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQuery RefreshQueriedOrientationEvaluateTypeTagQuery;
	
	/** <MovementMode, AimOffsetBlendSpace> */
	UPROPERTY(EditAnywhere, Category = "Animation")
	TMap<FGameplayTag, TObjectPtr<UAimOffsetBlendSpace>> ModdedAimOffsetBlendSpaces;
	
	/** Aim offset is enabled only when the tag query matches. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FGameplayTagQuery EnabledTagQuery;
	
private:
	struct FAnimData
	{
		FVSMovementOrientationEvaluateType CurrentOrientationEvaluateType = FVSMovementOrientationEvaluateType(EVSMovementRelatedOrientationType::None);

		/** [Yaw, Pitch] */
		FVector2D AimOffsetAngle = FVector2D::ZeroVector;
		UAimOffsetBlendSpace* CurrentAimOffsetBlendSpace = nullptr;
		bool bMatchesTagQuery = false;
	} AnimData;
};
