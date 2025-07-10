// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSGameplayLibrary.h"

#include "Types/VSGameplayTypes.h"

UVSGameplayLibrary::UVSGameplayLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSGameplayLibrary::IsInGame()
{
	return !GIsEditor || GIsPlayInEditorWorld || (GWorld && GWorld->HasBegunPlay());
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Query.Matches(GameplayTags, TagEvent);
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Queries.Matches(GameplayTags, TagEvent);
}

bool UVSGameplayLibrary::SweepSingleByShapeAndChannels(const UObject* WorldContext, FHitResult& OutHit, const FVector& Start, const FVector& End, const FQuat& Rotation, const FCollisionShape& Shape, const FCollisionResponseContainer& Channels, const FCollisionQueryParams& QueryParams, FCollisionResponseParams ResponseParams)
{
	if (!WorldContext || !WorldContext->GetWorld()) return false;

	for (const uint8 CollisionChannel : Channels.EnumArray)
	{
		if (CollisionChannel == ECR_Block)
		{
			WorldContext->GetWorld()->SweepSingleByChannel(
				OutHit,
				Start,
				End,
				Rotation,
				static_cast<ECollisionChannel>(CollisionChannel),
				Shape,
				QueryParams,
				ResponseParams);
		}

		if (OutHit.bBlockingHit) { return true; }
	}
	
	return false;
}

FVector UVSGameplayLibrary::SuggestVelocityForProjectileMovementByTime(const FVector& StartLocation, const FVector& EndLocation, const float MovementTime, const FVector& GravityDirection, float GravitySize)
{
	if (MovementTime <= 0.f) return FVector::ZeroVector;

	const FVector& NegativeGravityNormal = GravityDirection.GetSafeNormal();
	
	const FVector& DeltaLocation2D = FVector::VectorPlaneProject((EndLocation - StartLocation), NegativeGravityNormal);
	const FVector& DeltaLocationZ = (EndLocation - StartLocation).ProjectOnToNormal(NegativeGravityNormal);

	FVector AnsVelocity = FVector::ZeroVector;
	AnsVelocity += DeltaLocation2D / MovementTime;
	AnsVelocity += DeltaLocationZ / MovementTime - GravitySize * NegativeGravityNormal * MovementTime;

	return AnsVelocity;
}
