// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSGameplayLibrary.h"

#include <rapidjson/reader.h>

#include "EngineUtils.h"
#include "Components/BrushComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Types/VSGameplayTypes.h"

UVSGameplayLibrary::UVSGameplayLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSGameplayLibrary::IsInGame()
{
	return !GIsEditor || GIsPlayInEditorWorld || (GWorld && GWorld->HasBegunPlay());
}

float UVSGameplayLibrary::GetServerTimeSeconds(const UObject* WorldContext)
{
	AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContext);
	// APlayerState* FirstPlayerState = UGameplayStatics::GetPlayerState(WorldContext, 0);
	return GameState ? GameState->GetServerWorldTimeSeconds() : 0.f;
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQuery(const FVSGameplayTagEventQuery& Query, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Query.Matches(GameplayTags, TagEvent);
}

bool UVSGameplayLibrary::MatchesGameplayTagEventQueries(const FVSGameplayTagEventQueryContainer& Queries, const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent)
{
	return Queries.Matches(GameplayTags, TagEvent);
}

bool UVSGameplayLibrary::MatchesSceneComponentQuery(const USceneComponent* SceneComponent, const FVSSceneComponentQuery& Query)
{
	return Query.Matches(SceneComponent);
}

APawn* UVSGameplayLibrary::GetPawnFromSubObject(UObject* Object)
{
	if (!Object) return nullptr;
	if (APawn* Pawn = Cast<APawn>(Object))
	{
		return Pawn;
	}
	if (AController* Controller = Cast<AController>(Object))
	{
		return Controller->GetPawn();
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Object))
	{
		return PlayerState->GetPawn();
	}

	return GetPawnFromSubObject(Object->GetOuter());
}

AController* UVSGameplayLibrary::GetControllerFromSubObject(UObject* Object)
{
	if (!Object) return nullptr;
	if (AController* Controller = Cast<AController>(Object))
	{
		return Controller;
	}
	if (APawn* Pawn = Cast<APawn>(Object))
	{
		return Pawn->GetController();
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Object))
	{
		return PlayerState->GetOwningController();
	}

	return GetControllerFromSubObject(Object->GetOuter());
}

bool UVSGameplayLibrary::GetScreenViewportSize(APlayerController* PlayerController, FVector2D& OutPosition)
{
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient)
		{
			ViewportClient->GetViewportSize(OutPosition);
			return true;
		}
	}
	
	return false;
}

bool UVSGameplayLibrary::GetLocationUnderCursor(FVector& OutLocation, const FVSLocationUnderCursorQueryParams QueryParams)
{
	if (!QueryParams.IsValid()) return false;

	FVector2D CursorPosition = QueryParams.CursorPositionOverride;
	if (!QueryParams.bOverrideCursorPosition)
	{
		QueryParams.PlayerController->GetMousePosition(CursorPosition.X, CursorPosition.Y);
	}
	
	if (QueryParams.bTraceByCollision)
	{
		FHitResult HitResult;
		QueryParams.PlayerController->GetHitResultAtScreenPosition(CursorPosition, QueryParams.TraceType, true, HitResult);

		if (HitResult.IsValidBlockingHit())
		{
			OutLocation = HitResult.ImpactPoint;
			return true;
		}
	}

	if (QueryParams.bIntersectByPlane)
	{
		const FVector& NormalizedPlaneNormal = QueryParams.PlaneNormal.GetSafeNormal();

		FVector Location, Direction;

		UGameplayStatics::DeprojectScreenToWorld(QueryParams.PlayerController.Get(), CursorPosition, Location, Direction);

		float DotProduct = FVector::DotProduct(Direction, NormalizedPlaneNormal);

		/** Check if paralleled. */
		if (FMath::IsNearlyZero(DotProduct)) return false;
		
		float VerticalDistance = FVector::DotProduct(QueryParams.PlanePoint - Location, NormalizedPlaneNormal) / DotProduct;

		/** Check if this point is at the back of our viewport. */
		if (VerticalDistance < 0.f) return false;
	
		OutLocation = Location + Direction * VerticalDistance;
		return true;
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
	AnsVelocity += DeltaLocationZ / MovementTime - 0.5f * GravitySize * NegativeGravityNormal * MovementTime;

	return AnsVelocity;
}

APostProcessVolume* UVSGameplayLibrary::GetPostProcessVolumeAtLocation(UObject* WorldContext, const FVector& Location)
{
	if (!WorldContext) return nullptr;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	
	APostProcessVolume* BestVolume = nullptr;
	float BestPriority = -FLT_MAX;

	for (TActorIterator<APostProcessVolume> It(World); It; ++It)
	{
		APostProcessVolume* PPVolume = *It;
		if (!PPVolume || PPVolume->Priority < BestPriority) continue;

		bool bContainsPoint = false;
		if (PPVolume->bUnbound)
		{
			bContainsPoint = true;
		}
		else if (PPVolume->GetBrushComponent()->Bounds.GetBox().IsInside(Location))
		{
			bContainsPoint = true;
		}

		if (bContainsPoint)
		{
			BestPriority = PPVolume->Priority;
			BestVolume = PPVolume;
		}
	}

	return BestVolume;
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
