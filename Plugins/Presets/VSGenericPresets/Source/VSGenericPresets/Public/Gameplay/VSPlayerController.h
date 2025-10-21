// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "UObject/Object.h"
#include "VSPlayerController.generated.h"

class UVSObjectFeatureComponent;

/**
 * 
 */
UCLASS()
class VSGENERICPRESETS_API AVSPlayerController : public APlayerController, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	
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
