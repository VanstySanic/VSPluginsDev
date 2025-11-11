// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "VSCharacterMovementAnimInterface.h"
#include "VSCharacterMovementInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/VSControlRotationFeatureInterface.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSCharacter.generated.h"

class UVSObjectFeatureComponent;

UCLASS()
class VSGENERICPRESETS_API AVSCharacter : public ACharacter, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface, public IVSCharacterMovementInterface, public IVSCharacterMovementAnimInterface, public IVSControlRotationFeatureInterface
{
	GENERATED_UCLASS_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Components")
	UVSObjectFeatureComponent* GetFeatureComponent() const;

protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;
	virtual UVSCharacterMovementFeatureAgent* GetMovementAgentFeature_Implementation() const override;
	virtual UVSCharacterMovementAnimFeatureAgent* GetCharacterMovementAnimFeatureAgent_Implementation() const override;
	virtual UVSControlRotationFeature* GetControlRotationFeature_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;
};
