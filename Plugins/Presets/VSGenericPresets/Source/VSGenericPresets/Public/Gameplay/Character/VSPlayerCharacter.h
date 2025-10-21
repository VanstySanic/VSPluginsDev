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
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;
};
