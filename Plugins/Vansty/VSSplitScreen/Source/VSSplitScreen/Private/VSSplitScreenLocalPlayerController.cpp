// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreen/Public/VSSplitScreenLocalPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

AVSSplitScreenLocalPlayerController::AVSSplitScreenLocalPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = false;
}

void AVSSplitScreenLocalPlayerController::InitPlayerState()
{
	if ( GetNetMode() != NM_Client )
	{
		UWorld* const World = GetWorld();
		const AGameModeBase* GameMode = World ? World->GetAuthGameMode() : NULL;

		// If the GameMode is null, this might be a network client that's trying to
		// record a replay. Try to use the default game mode in this case so that
		// we can still spawn a PlayerState.
		if (GameMode == NULL)
		{
			const AGameStateBase* const GameState = World ? World->GetGameState() : NULL;
			GameMode = GameState ? GameState->GetDefaultGameMode() : NULL;
		}

		if (GameMode != NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map

			TSubclassOf<APlayerState> PlayerStateClassToSpawn = APlayerState::StaticClass();
			if (PlayerStateClassToSpawn.Get() == nullptr)
			{
				UE_LOG(LogPlayerController, Log, TEXT("AController::InitPlayerState: the PlayerStateClass of game mode %s is null, falling back to APlayerState."), *GameMode->GetName());
				PlayerStateClassToSpawn = APlayerState::StaticClass();
			}

			SetPlayerState(World->SpawnActor<APlayerState>(PlayerStateClassToSpawn, SpawnInfo));
	
			// force a default player name if necessary
			if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
			{
				// don't call SetPlayerName() as that will broadcast entry messages but the GameMode hasn't had a chance
				// to potentially apply a player/bot name yet
				
				PlayerState->SetPlayerNameInternal(GameMode->DefaultPlayerName.ToString());
			}
		}
	}
}