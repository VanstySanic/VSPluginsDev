// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VSCharacterMovementAnimFeature.h"
#include "VSCharacterMovementAnimFeatureAgent.generated.h"

class UVSGameplayTagController;
class UCharacterMovementComponent;
class UVSCharacterMovementFeatureAgent;

/**
 * 
 */
UCLASS(DisplayName = "Feautre.ChrMovAnim.Agent", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSCharacterMovementAnimFeatureAgent : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSCharacterMovementAnimFeature;

protected:
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void EndPlay_Implementation() override;
	virtual void UpdateAnimation_Implementation(float DeltaTime) override;
	virtual void UpdateAnimationThreadSafe_Implementation(float DeltaTime) override;
	virtual void OnMovementTagsUpdated_Implementation() override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	/**
	 * When calculating direction (velocity or acceleration, etc.),
	 * the angle will work as the buffer to avoid repetitive direction changes. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DirectionBufferAngle = 4.5f;
	
private:
	struct FAnimData
	{
		/** Whether the MovementMode is changed in this frame. */
		bool bMovementModeChangedThisFrame = false;
		
		/** The direction between the velocity and character rotation. Moving only, wall adjusted, 2D. */
		FGameplayTag AnimVelocityDirectionToCharacter2D;
		FGameplayTag CachedMovementMode;
	} AnimData;

	TWeakObjectPtr<ACharacter> CharacterPrivate;
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovementComponentPrivate;
	TWeakObjectPtr<UVSCharacterMovementFeatureAgent> ChrMovAgentFeaturePrivate;
	TWeakObjectPtr<UVSGameplayTagController> GameplayTagControllerPrivate;
};
