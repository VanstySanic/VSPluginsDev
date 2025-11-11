// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacter.h"
#include "VSPlayerCharacter.generated.h"

UCLASS()
class VSGENERICPRESETS_API AVSPlayerCharacter : public AVSCharacter
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	/** This is call when the actor has begun play, and the player state && player controller (if exists) are valid. */
	UFUNCTION(BlueprintNativeEvent, Category = "Gameplay")
	void SetupAtExpectedTime();
	
private:
	void CheckExpectedTimeToSetup();


private:
	bool bHasBeenSetupAtTheBestTime = false;
};
