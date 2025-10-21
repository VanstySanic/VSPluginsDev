// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSPlayerPawn.generated.h"

class UAbilitySystemComponent;
class UVSObjectFeatureComponent;

UCLASS()
class VSGENERICPRESETS_API AVSPlayerPawn : public APawn, public IAbilitySystemInterface, public IVSGameplayTagControllerInterface
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	
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
