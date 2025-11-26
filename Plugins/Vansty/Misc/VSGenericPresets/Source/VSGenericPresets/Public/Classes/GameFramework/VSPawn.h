// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverSimulationTypes.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "VSPawn.generated.h"

UCLASS()
class VSGENERICPRESETS_API AVSPawn : public APawn, public IMoverInputProducerInterface, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()

public:

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMoverComponent> MoverComponent;
};
