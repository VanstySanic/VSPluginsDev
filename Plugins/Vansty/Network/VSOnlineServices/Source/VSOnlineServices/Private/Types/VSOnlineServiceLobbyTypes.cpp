// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSOnlineServiceLobbyTypes.h"

using namespace UE::Online;

FVSCreateLobbyParams::FVSCreateLobbyParams(const FCreateLobby::Params& InParams)
{
	LocalAccountId = InParams.LocalAccountId.GetHandle();
	LocalName = InParams.LocalName;
	SchemaId = InParams.SchemaId;
	bPresenceEnabled = InParams.bPresenceEnabled;
	MaxMembers = InParams.MaxMembers;
	JoinPolicy = (EVSLobbyJoinPolicy::Type)(uint8)InParams.JoinPolicy;

	for (const TPair<FName, FSchemaVariant>& Attribute : InParams.Attributes)
	{
		Attributes.Emplace(Attribute.Key, Attribute.Value.GetString());
	}

	for (const TPair<FName, FSchemaVariant>& UserAttribute : InParams.UserAttributes)
	{
		UserAttributes.Emplace(UserAttribute.Key, UserAttribute.Value.GetString());
	}
}

FCreateLobby::Params FVSCreateLobbyParams::ToParams() const
{
	FCreateLobby::Params OutParams;
	
	OutParams.LocalAccountId = FAccountId(EOnlineServices::Epic, LocalAccountId);
	OutParams.LocalName = LocalName;
	OutParams.SchemaId = FSchemaId(SchemaId);
	OutParams.bPresenceEnabled = bPresenceEnabled;
	OutParams.MaxMembers = MaxMembers;
	OutParams.JoinPolicy = (ELobbyJoinPolicy)(uint8)JoinPolicy;

	for (const TPair<FName, FString>& Attribute : Attributes)
	{
		OutParams.Attributes.Emplace(FSchemaAttributeId(Attribute.Key), FSchemaVariant(Attribute.Value));
	}
	
	for (const TPair<FName, FString>& UserAttribute : UserAttributes)
	{
		OutParams.UserAttributes.Emplace(FSchemaAttributeId(UserAttribute.Key), FSchemaVariant(UserAttribute.Value));
	}

	return OutParams;
}
