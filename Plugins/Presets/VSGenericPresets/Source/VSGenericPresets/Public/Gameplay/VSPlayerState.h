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
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void SetOwner(AActor* NewOwner) override;
	virtual void OnRep_Owner() override;
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Components")
	UVSObjectFeatureComponent* GetFeatureComponent() const;
	
protected:
	/** This is call when the actor has begun play, and the player state && player controller (if exists) are valid. */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void SetupAtExpectedTime();
	
private:
	void CheckExpectedTimeToSetup();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;

private:
	bool bHasBeenSetupAtExpectedTime = false;
};
