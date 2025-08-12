// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_OrientationGravityAdjustment.generated.h"

/**
 * Adjust the character's orientation by making the down vector towards the gravity direction.
 * The update should not be executed while turning in place.
 */
UCLASS(DisplayName = "Feature.ChrMov.Orientation.GravityAdjustment")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_OrientationGravityAdjustment : public UVSCharacterMovementFeature
{
	GENERATED_BODY()

public:
	UVSChrMovFeature_OrientationGravityAdjustment();

protected:
	virtual void BeginPlay_Implementation() override;
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	// float OrientationLagSpeed = 10.f;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	// float LagMaxTimeSubstepping = 0.0166667f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bAdjustControlRotation = true;
    	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAntiGravity = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVSGameplayTagEventQueryContainer MovementStateQuerySettings; 

	
private:
	struct FMovementData
	{
		bool bMatchesTagQuery = false;
	} MovementData;
};
