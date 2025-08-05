// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Framework/VSObjectFeature.h"
#include "GameplayTags.h"
#include "VSCharacterMovementInterface.h"
#include "Types/VSCharacterMovementTypes.h"
#include "VSCharacterMovementFeature.generated.h"

class UVSCharacterMovementComponent;
class UVSGameplayTagController;
class UVSChrMovCapsuleComponent;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Feature.ChrMov.Base")
class VSMOVEMENTSYSTEM_API UVSCharacterMovementFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSCharacterMovementFeatureAgent;
	friend class UVSCharacterMovementComponent;
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UCharacterMovementComponent* GetCharacterMovement() const;

	UFUNCTION(BlueprintCallable, Category = "References")
	ACharacter* GetCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category = "References")
	AController* GetController() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSCharacterMovementFeatureAgent* GetMovementFeatureAgent() const { return ChrMovFeatureAgentPrivate.Get(); }

	UFUNCTION(BlueprintCallable, Category = "References")
	UVSGameplayTagController* GetGameplayTagController() const;

	/** Get the capsule component that takes place of the character's capsule collision. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UVSChrMovCapsuleComponent* GetMovementCapsuleComponent() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetMovementMode() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetPrevMovementMode() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "InMovementMode"))
	void SetMovementMode(const FGameplayTag& InMovementMode, bool bReplicated = false);
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocity() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocity2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocityWallAdjusted2D() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetVelocityZ() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetSpeed2D() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetSpeedWallAdjusted2D() const;
	
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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetRootLocation() const;


	UFUNCTION(BlueprintCallable, Category = "Movement")
	FRotator EvaluateOrientation(const FVSMovementOrientationEvaluateParams& Params);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	void UpdateMovement(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	bool CanUpdateMovement() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementTagsUpdated();

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementTagEventNotified(const FGameplayTag& TagEvent);

private:
	UFUNCTION(Server, Reliable)
	void SetMovementMode_Server(const FGameplayTag& InMovementMode);
	
	void SetMovementModeInternal(const FGameplayTag& InMovementMode);

private:
	TWeakObjectPtr<UVSCharacterMovementFeatureAgent> ChrMovFeatureAgentPrivate;
};
