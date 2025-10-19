// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacter.h"
#include "VSAICharacter.generated.h"

class UAbilitySystemComponent;
class UVSObjectFeatureComponent;

UCLASS()
class VSGENERICPRESETS_API AVSAICharacter : public AVSCharacter
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
