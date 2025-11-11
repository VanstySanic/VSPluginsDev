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
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Components")
	UVSObjectFeatureComponent* GetFeatureComponent() const;

protected:
	/** This is call when the actor has begun play, and the player state is valid. */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void SetupAtExpectedTime();
	
private:
	void CheckExpectedTimeToSetup();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;

private:
	bool bHasBeenSetupAtExpectedTime = false;
};
