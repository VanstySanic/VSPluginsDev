// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSCharacter.generated.h"

class UVSObjectFeatureComponent;

UCLASS()
class VSGENERICPRESETS_API AVSCharacter : public ACharacter, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface
{
	GENERATED_UCLASS_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Components")
	UVSObjectFeatureComponent* GetFeatureComponent() const;

protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;
};
