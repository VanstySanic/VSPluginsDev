// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreenPlayerFeature.h"
#include "VSSplitScreenSubsystemGI.h"
#include "VSSplitScreenSubsystemW.h"
#include "Classes/Features/VSAlphaBlendFeature.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"

UVSSplitScreenPlayerFeature::UVSSplitScreenPlayerFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSplitScreenPlayerFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	APlayerState* PlayerState = GetTypedOuter<APlayerState>();
	if (!PlayerState)
	{
		if (APawn* Pawn = UVSActorLibrary::GetPawnFromActor(GetOwnerActor()))
		{
			PlayerState = Pawn->GetPlayerState();
		}
	}
	check(PlayerState);
	PlayerState->OnPawnSet.AddDynamic(this, &UVSSplitScreenPlayerFeature::OnPlayerStatePawnSet);

	const int32 PlayerID = UVSActorLibrary::IsActorNetLocal(PlayerState) ? 0 : 1;
	SplitScreenPlayer = UVSSplitScreenSubsystemGI::Get(this)->GetPlayerFormID(PlayerID);

	UVSSplitScreenSubsystemW::Get(this)->SetPlayerState(SplitScreenPlayer, PlayerState);
}

FVSSplitScreenPlayerData UVSSplitScreenPlayerFeature::GetSplitScreenPlayerData() const
{
	return UVSSplitScreenSubsystemW::Get(this)->GetSplitScreenPlayerData(SplitScreenPlayer);
}

void UVSSplitScreenPlayerFeature::OnPlayerStatePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (!Player || !NewPawn) return;
	UVSSplitScreenSubsystemW::Get(this)->SetPlayerViewTargetWithBlend(SplitScreenPlayer, NewPawn);
	Player->OnPawnSet.RemoveDynamic(this, &UVSSplitScreenPlayerFeature::OnPlayerStatePawnSet);
}
