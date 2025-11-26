// Copyright VanstySanic. All Rights Reserved.

#include "Classes/GameFramework/VSPawn.h"
#include "MoverComponent.h"

AVSPawn::AVSPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MoverComponent = CreateDefaultSubobject<UMoverComponent>(TEXT("Mover"));
}

