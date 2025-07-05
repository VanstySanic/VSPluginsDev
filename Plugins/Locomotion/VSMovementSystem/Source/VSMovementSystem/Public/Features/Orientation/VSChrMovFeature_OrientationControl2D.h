// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_OrientationControl2D.generated.h"

/**
 * Gives convenient features for 2D orientation control.
 * This works for movement modes that just requires simple orientation or automatic management.
 * 2D means the character's local space yaw.
 */
UCLASS(DisplayName = "Feature.ChrMov.Orientation.Control2D")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_OrientationControl2D : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	FGameplayTag GetMovingOrientationEvaluateType() const { return MovementData.CurrentMovingOrientationEvaluateType; }

	UFUNCTION(BlueprintCallable, Category = "Orientation")
	FGameplayTag GetIdleOrientationEvaluateType() const { return MovementData.CurrentIdleOrientationEvaluateType; }
	
protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

private:
	void UpdateTagQueryStates(const FGameplayTag& TagEvent);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bMovingRequireInput = true;
	
	/** Lag the moving orientation 2D to the desired. 0.f means no lag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovingOrientationLagSpeed = 10.f;

	/** Lag the idle orientation 2D to the desired. 0.f means no lag. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float IdleOrientationLagSpeed = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LagMaxTimeSubstepping = 0.0166667f;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FGameplayTag DefaultMovingOrientationEvaluateType = EVSOrientationEvaluateType::Control;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FGameplayTag DefaultIdleOrientationEvaluateType = EVSOrientationEvaluateType::None;
	
	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FGameplayTag, FVSGameplayTagEventQuery> QueriedMovingOrientationEvaluateTypes;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FGameplayTag, FVSGameplayTagEventQuery> QueriedIdleOrientationEvaluateTypes;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQueryContainer RefreshQueriedMovingOrientationEvaluateTypeQueries;
	
	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQueryContainer RefreshQueriedIdleOrientationEvaluateTypeQueries;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	FVSGameplayTagEventQueryContainer MovingTagQueries; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	FVSGameplayTagEventQueryContainer IdleTagQueries;
	
private:
	struct FMovementData
	{
		FGameplayTag CurrentMovingOrientationEvaluateType = EVSOrientationEvaluateType::None;
		FGameplayTag CurrentIdleOrientationEvaluateType = EVSOrientationEvaluateType::None;

		bool bMatchesMovingTagQuery = false;
		bool bMatchesIdleTagQuery = false;
	} MovementData;
};
