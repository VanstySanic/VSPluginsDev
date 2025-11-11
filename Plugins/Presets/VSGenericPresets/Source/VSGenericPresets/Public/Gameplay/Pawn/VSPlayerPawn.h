// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/VSControlRotationFeatureInterface.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSPlayerPawn.generated.h"

class UAbilitySystemComponent;
class UVSObjectFeatureComponent;

UCLASS()
class VSGENERICPRESETS_API AVSPlayerPawn : public APawn, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface, public IVSControlRotationFeatureInterface
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	virtual void OnRep_Controller() override;
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;
	virtual UVSControlRotationFeature* GetControlRotationFeature_Implementation() const override;

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
	TObjectPtr<UVSObjectFeatureComponent> FeatureComponent;

private:
	bool bHasBeenSetupAtExpectedTime = false;
};
