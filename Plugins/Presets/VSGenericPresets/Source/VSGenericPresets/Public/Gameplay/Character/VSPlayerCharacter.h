// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCharacter.h"
#include "VSPlayerCharacter.generated.h"

UCLASS()
class VSGENERICPRESETS_API AVSPlayerCharacter : public AVSCharacter
{
	GENERATED_UCLASS_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual UVSGameplayTagController* GetGameplayTagController_Implementation() const override;
};
