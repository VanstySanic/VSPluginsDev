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

	UFUNCTION(BlueprintCallable, Category = "Orientation", meta = (AutoCreateRefTerm = "Type"))
	void SetOverridenMovingOrientationEvaluateType(const FGameplayTag& Type);
	
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	void SetOverrideMovingOrientationEvaluateType(const bool bOverride);

	UFUNCTION(BlueprintCallable, Category = "Orientation", meta = (AutoCreateRefTerm = "Type"))
	void SetOverridenIdleOrientationEvaluateType(const FGameplayTag& Type);
	
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	void SetOverrideIdleOrientationEvaluateType(const bool bOverride);
	
protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagsUpdated_Implementation() override;
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
	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FGameplayTag, FVSGameplayTagEventQuery> QueriedMovingOrientationEvaluateTypes;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FGameplayTag, FVSGameplayTagEventQuery> QueriedIdleOrientationEvaluateTypes;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQuery RefreshQueriedMovingOrientationEvaluateTypeQuery;
	
	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQuery RefreshQueriedIdleOrientationEvaluateTypeQuery;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FGameplayTag OverridenMovingOrientationEvaluateType = EVSOrientationEvaluateType::Control;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FGameplayTag OverridenIdleOrientationEvaluateType = EVSOrientationEvaluateType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	bool bOverrideMovingOrientationEvaluateType = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	bool bOverrideIdleOrientationEvaluateType = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	FVSGameplayTagEventQuery MovingTagQuerySettings; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	FVSGameplayTagEventQuery IdleTagQuerySettings;
	
private:
	struct FMovementData
	{
		FGameplayTag CurrentMovingOrientationEvaluateType = EVSOrientationEvaluateType::None;
		FGameplayTag CurrentTaggedMovingOrientationEvaluateType = EVSOrientationEvaluateType::None;
		FGameplayTag CurrentIdleOrientationEvaluateType = EVSOrientationEvaluateType::None;
		FGameplayTag CurrentTaggedIdleOrientationEvaluateType = EVSOrientationEvaluateType::None;

		bool bMatchesMovingTagQuery = false;
		bool bMatchesIdleTagQuery = false;
	} MovementData;
};
