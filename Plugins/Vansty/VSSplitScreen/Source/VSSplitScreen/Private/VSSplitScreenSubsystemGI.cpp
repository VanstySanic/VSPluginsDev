// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreenSubsystemGI.h"

UVSSplitScreenSubsystemGI::UVSSplitScreenSubsystemGI()
{
	SetPlayer(0, EVSSplitScreenPlayer::PlayerOne);
	SetPlayer(1, EVSSplitScreenPlayer::PlayerTwo);
}

UVSSplitScreenSubsystemGI* UVSSplitScreenSubsystemGI::Get(const UObject* WorldContext)
{
	if (!WorldContext || !WorldContext->GetWorld() || !WorldContext->GetWorld()->GetGameInstance()) return nullptr;
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UVSSplitScreenSubsystemGI>();
}

void UVSSplitScreenSubsystemGI::SetPlayer(int32 PlayerID, EVSSplitScreenPlayer::Type Player)
{
	IdentifiedPlayers.Emplace(PlayerID, Player);
	PlayeredIds.Emplace(Player, PlayerID);
}

EVSSplitScreenPlayer::Type UVSSplitScreenSubsystemGI::GetPlayerFormID(int32 ID)
{
	return IdentifiedPlayers.FindRef(ID);
}

int32 UVSSplitScreenSubsystemGI::GetIDFormPlayer(EVSSplitScreenPlayer::Type Player)
{
	return PlayeredIds.FindRef(Player);
}
