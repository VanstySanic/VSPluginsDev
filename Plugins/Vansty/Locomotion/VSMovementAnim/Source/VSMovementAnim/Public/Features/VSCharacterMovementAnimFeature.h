// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Classes/Features/VSObjectFeature.h"
#include "VSCharacterMovementAnimFeature.generated.h"

class UVSGameplayTagController;
class UVSCharacterMovementFeatureAgent;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Feature.ChrMovAnim.Base", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSCharacterMovementAnimFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSCharacterMovementAnimFeatureAgent;

protected:
	virtual void Initialize_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "References")
	ACharacter* GetCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UCharacterMovementComponent* GetCharacterMovement() const;
	
	UFUNCTION(BlueprintCallable, Category = "References")
	AController* GetController() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSCharacterMovementAnimFeatureAgent* GetAnimFeatureAgent() const { return AnimFeatureAgentPrivate.Get(); }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSCharacterMovementFeatureAgent* GetMovementFeatureAgent() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSGameplayTagController* GetGameplayTagController() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetMovementMode() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetPrevMovementMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocity() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocity2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetAnimVelocity2D() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocityZ() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetSpeed2D() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetAnimSpeed2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetSpeedZ() const;
	
	/** The acceleration value scaled by the input amount. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetMovementInput() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetMovementInput2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetRealAcceleration() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetRealAcceleration2D() const;

	/** Has 2D speed. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsMoving2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsMovingAgainstWall2D() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool HasMovementInput() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool HasMovementInput2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetScale3D() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetUpDirection() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetGravityDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FRotator GetControlRotation() const;

	
	/** The direction between the velocity and character rotation. Moving only, wall adjusted, 2D. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetAnimVelocityDirectionToCharacter2D() const;

	
	/** Need to be manually called in anim instance. Call on the agent will update sub anim features. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void UpdateAnimation(float DeltaTime);

	/** Need to be manually called in anim instance. Call on the agent will update sub anim features. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void UpdateAnimationThreadSafe(float DeltaTime);
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementTagsUpdated();

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementTagEventNotified(const FGameplayTag& TagEvent);

private:
	TWeakObjectPtr<UVSCharacterMovementAnimFeatureAgent> AnimFeatureAgentPrivate;
};
