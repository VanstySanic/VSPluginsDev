// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Online/Lobbies.h"
#include "UObject/Object.h"
#include "VSOnlineServiceLobbyTypes.generated.h"

UENUM(BlueprintType)
namespace EVSLobbyJoinPolicy
{
	enum Type
	{
		/** 
		 * Lobby can be found through searches based on attribute matching,
		 * by knowing the lobby id, or by invitation.
		 */
		PublicAdvertised,

		/** Lobby may be joined by knowing the lobby id or by invitation. */
		PublicNotAdvertised,

		/** Lobby may only be joined by invitation. */
		InvitationOnly,
	};
}

USTRUCT(BlueprintType)
struct FVSCreateLobbyParams
{
	GENERATED_BODY()

	FVSCreateLobbyParams() {}
	FVSCreateLobbyParams(const UE::Online::FCreateLobby::Params& InParams);
	UE::Online::FCreateLobby::Params ToParams() const;

	/** The local user agent which will perform the action. */
	int32 LocalAccountId = 0;

	/** The local name for the lobby. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LocalName;

	/** Whether this lobby should be set as the user's new presence lobby. False by default */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPresenceEnabled = false;

	/** The maximum number of members who can fit in the lobby. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMembers = 0;

	/** Initial join policy setting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSLobbyJoinPolicy::Type> JoinPolicy = EVSLobbyJoinPolicy::PublicAdvertised;

	/** The schema which will be applied to the lobby. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SchemaId = FName("UnnamedSchema");
	
	/** Initial attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> Attributes;

	/** Initial user attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> UserAttributes;
};
