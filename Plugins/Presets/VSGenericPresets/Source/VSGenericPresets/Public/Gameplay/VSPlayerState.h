// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSPlayerState.generated.h"

class UVSObjectFeatureComponent;

/**
 * 
 */
UCLASS()
class VSGENERICPRESETS_API AVSPlayerState : public APlayerState, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void PostInitializeComponents() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Components")
	UVSObjectFeatureComponent* GetFeatureComponent() const;

protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;
};
