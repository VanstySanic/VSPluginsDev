// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Online/Sessions.h"
#include "UObject/Object.h"
#include "VSOnlineServiceSessionTypes.generated.h"

/** Set of options to reflect how a session may be discovered in searches and joined */
UENUM(BlueprintType)
namespace EVSSessionJoinPolicy
{
	enum Type
	{
		/** The session will appear on searches an may be joined by anyone */
		Public,

		/** The session will not appear on searches and may only be joined via presence (if enabled) or invitation */
		FriendsOnly,

		/** The session will not appear on searches and may not be joined via presence, only via invitation */
		InviteOnly
	};
}

USTRUCT(Blueprintable)
struct FVSCreateSessionSessionParams
{
	GENERATED_BODY()

	FVSCreateSessionSessionParams() {}
	FVSCreateSessionSessionParams(const UE::Online::FCreateSession::Params& InParams);
	UE::Online::FCreateSession::Params ToParams() const;

	int32 LocalAccountId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SessionName = NAME_None;
	
	/** In platforms that support this feature, it will set the session id to this value. Might be subject to minimum and maximum length restrictions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SessionIdOverride;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLANSession = false;
	
	/* Maximum number of slots for session members. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumMaxConnections = 0;
	
	/** Whether this session should be set as the local user's new presence session. False by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPresenceEnabled = false;
	
	/** Whether this session will allow sanctioned players to join it. True by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowSanctionedPlayers = false;
	
	/** Whether this is a secure session protected by anti-cheat services. False by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAntiCheatProtected = false;

	/* Enum value describing the level of restriction to join the session. Public by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSSessionJoinPolicy::Type> JoinPolicy = EVSSessionJoinPolicy::Public;
	
	/* The name for the schema which will be applied to the session's user-defined attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SchemaName = FName("UnnamedSchema");

	/** Settings to define session properties during creation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> CustomSettings;
};

