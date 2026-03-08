// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSMovementFeatureBase.h"
#include "VSCharacterMovementFeature.generated.h"

class UVSCharacterMovementFeatureAgent;
class UCharacterMovementComponent;

/**
 * Base movement feature that exposes ACharacter/UCharacterMovementComponent data.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Character.Base")
class VSCHARACTERMOVEMENTSYSTEM_API UVSCharacterMovementFeature : public UVSMovementFeatureBase
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSObjectFeatureInterface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeatureInterface
	
	//~ Begin UVSMovementFeatureBase
	virtual FVector GetAngularVelocity_Implementation() const override;
	virtual FVector GetAcceleration_Implementation() const override;
	virtual FVector GetMovementInput_Implementation() const override;
	virtual USceneComponent* GetUpdatedComponent_Implementation() const override;
	virtual UPrimitiveComponent* GetMovementBase_Implementation() const override;
	virtual FName GetMovementBaseBoneName_Implementation() const override;
	virtual FGameplayTag GetMovementMode_Implementation() const override;
	//~ End UVSMovementFeatureBase

	
public:
	UFUNCTION(BlueprintCallable, Category = "References")
	ACharacter* GetCharacter() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	UCharacterMovementComponent* GetCharacterMovement() const;

	UFUNCTION(BlueprintCallable, Category = "References")
	UVSCharacterMovementFeatureAgent* GetCharacterMovementFeatureAgent() const;
	
private:
	TWeakObjectPtr<UVSCharacterMovementFeatureAgent> CharacterMovementFeatureAgentPrivate;
};


/** ------------------------------------------------------------------------- **/


/**
 * Agent feature that derives movement data from CharacterMovement and broadcasts mode changes.
 */
UCLASS(Abstract, DisplayName = "VS.Feature.Movement.Character.Agent")
class VSCHARACTERMOVEMENTSYSTEM_API UVSCharacterMovementFeatureAgent : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

	friend class UVSCharacterMovementFeature;

public:
	//~ Begin UVSObjectFeature Interface
	virtual void Initialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	//~ End UVSObjectFeature Interface
	
	//~ Begin IVSGameplayTagFeatureInterface
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ End IVSGameplayTagFeatureInterface
	
	//~ Begin UVSMovementFeatureBase Interface
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	//~ End UVSMovementFeatureBase Interface

private:
	/** Handles movement-mode change notifications from ACharacter. */
	UFUNCTION()
	void OnCharacterMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

private:
	TWeakObjectPtr<ACharacter> CharacterPrivate;

	FVector RealMovementAcceleration = FVector::ZeroVector;
	FVector RealMovementAngularVelocity = FVector::ZeroVector;
};
