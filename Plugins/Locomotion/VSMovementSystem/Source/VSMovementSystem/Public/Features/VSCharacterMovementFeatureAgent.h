// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSCharacterMovementFeature.h"
#include "VSCharacterMovementFeatureAgent.generated.h"

class UVSGameplayTagController;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Agent")
class VSMOVEMENTSYSTEM_API UVSCharacterMovementFeatureAgent : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSCharacterMovementFeature;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovementModeChangedDelegate, const FGameplayTag&, NewMovementMode, const FGameplayTag&, PrevMovementMode);

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;

private:
	void CheckMovingAgainstWall2D();

	UFUNCTION()
	void OnCharacterMovementChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
	
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnMovementModeChangedDelegate OnMovementModeChanged;
	
private:
	TWeakObjectPtr<ACharacter> CharacterPrivate;
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponentPrivate;
	TWeakObjectPtr<UVSGameplayTagController> GameplayTagControllerPrivate;
	
	struct FMovementData
	{
		FMovementData()
			:bIsMovingAgainstWall2D(false)
		{
		}
		
		FGameplayTag PrevMovementMode;
		FVector RealAcceleration = FVector::ZeroVector;
		uint8 bIsMovingAgainstWall2D : 1;
	} MovementData;
};
