// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSActorLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Classes/Features/VSObjectFeature.h"
#include "Classes/Features/VSObjectFeatureComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Libraries/VSObjectLibrary.h"

UVSActorLibrary::UVSActorLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSActorLibrary::IsActorNetLocal(AActor* Actor)
{
	if (!Actor) return false;

	if (Actor->HasLocalNetOwner()) return true;
	if (Actor->GetLocalRole() == ROLE_AutonomousProxy) return true;
	if (Actor->GetLocalRole() == ROLE_Authority && Actor->GetRemoteRole() == ROLE_AutonomousProxy) return true;
	
	return false;
}

bool UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(AActor* Actor)
{
	if (!Actor) return false;
	return (Actor->HasAuthority() || Actor->GetLocalRole() == ROLE_AutonomousProxy);
}

AActor* UVSActorLibrary::DuplicateActor(AActor* Actor, const FTransform& SpawnTransform)
{
	if (!Actor) return nullptr;
	UWorld* World = Actor->GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.Template = Actor;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Name = MakeUniqueObjectName(World, Actor->GetClass(), Actor->GetFName());

	AActor* NewActor = World->SpawnActor<AActor>(Actor->GetClass(), Actor->GetActorTransform(), Params);
    
	return NewActor;
}

UActorComponent* UVSActorLibrary::GetActorComponentByName(const AActor* Actor, FName ComponentName)
{
	if (!Actor) return nullptr;
	const TSet<UActorComponent*>& Components = Actor->GetComponents();
	for (UActorComponent* Component : Components)
	{
		if (Component && Component->GetFName() == ComponentName)
		{
			return Component;
		}
	}

	return nullptr;
}

UCameraComponent* UVSActorLibrary::GetActiveCameraFromActor(const AActor* Actor)
{
	if (!Actor) return nullptr;
	TArray<UCameraComponent*> Components;
	Actor->GetComponents(UCameraComponent::StaticClass(), Components);
	for (UCameraComponent* Component : Components)
	{
		if (Component && Component->IsActive())
		{
			return Component;
		}
	}
	
	if (const APawn* Pawn = Cast<ACharacter>(Actor))
	{
		if (AController* Controller = Pawn->GetController())
		{
			return GetActiveCameraFromActor(Controller->GetViewTarget());
		}		
	}

	if (const AController* Controller = Cast<AController>(Actor))
	{
		return GetActiveCameraFromActor(Controller->GetViewTarget());
	}	

	if (const APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		if (APlayerController* PlayerController = PlayerState->GetPlayerController())
		{
			return GetActiveCameraFromActor(PlayerController->GetViewTarget());
		}
	}
	
	return nullptr;
}

UVSGameplayTagController* UVSActorLibrary::GetGameplayTagControllerFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	UVSGameplayTagController* GameplayTagController = nullptr;
	if (!GameplayTagController)
	{
		if (Actor->GetClass()->ImplementsInterface(UVSGameplayTagControllerInterface::StaticClass()))
		{
			GameplayTagController = IVSGameplayTagControllerInterface::Execute_GetGameplayTagController(Actor);
		}
	}
	if (!GameplayTagController)
	{
		GameplayTagController = UVSObjectLibrary::FindFeatureByClassFromObject<UVSGameplayTagController>(Actor);
	}
	return GameplayTagController;
}

UAbilitySystemComponent* UVSActorLibrary::GetAbilitySystemComponentFormActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (UAbilitySystemComponent* AbilitySystemComponent = Actor->FindComponentByClass<UAbilitySystemComponent>())
	{
		return AbilitySystemComponent;
	}

	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		if (AController* Controller = Pawn->GetController())
		{
			if (UAbilitySystemComponent* AbilitySystemComponent = Controller->FindComponentByClass<UAbilitySystemComponent>())
			{
				return AbilitySystemComponent;
			}
		}
			
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(Pawn))
		{
			if (APlayerState* PlayerState = Cast<APlayerState>(OwnerCharacter->GetPlayerState()))
			{
				if (UAbilitySystemComponent* AbilitySystemComponent = PlayerState->FindComponentByClass<UAbilitySystemComponent>())
				{
					return AbilitySystemComponent;

				}
			}
		}
	}

	return nullptr;
}

APawn* UVSActorLibrary::GetPawnFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn;
	}
	if (AController* Controller = Cast<AController>(Actor))
	{
		return Controller->GetPawn();
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		return PlayerState->GetPawn();
	}

	return GetPawnFromActor(Actor->GetOwner());
}

AController* UVSActorLibrary::GetControllerFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (AController* Controller = Cast<AController>(Actor))
	{
		return Controller;
	}
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetController();
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		return PlayerState->GetOwningController();
	}

	return GetControllerFromActor(Actor->GetOwner());
}

bool UVSActorLibrary::IsCharacterOnWalkableFloor(const ACharacter* Character, const float ToleranceToFloor, const bool bConsiderVerticalSpeedPrediction, const bool bConsiderCollisionOffset)
{
	if (!Character) return false;

	/* Character info. */
	const FVector& CharacterUpVector = Character->GetActorUpVector();
	const float CharacterScaleZ = Character->GetActorScale3D().Z;
	const float WalkableAngle = Character->GetCharacterMovement()->GetWalkableFloorAngle();
	const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector& LowerSphereCenter = Character->GetActorLocation() - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere() * CharacterUpVector;

	float DownTraceDistance = (bConsiderCollisionOffset ? 2.56f : 0.f)
		+ ((bConsiderVerticalSpeedPrediction && Character->GetVelocity().Dot(CharacterUpVector) < 0.f) ? (Character->GetVelocity().ProjectOnToNormal(CharacterUpVector).Size() * Character->GetWorld()->GetDeltaSeconds()) : 0.f)
		+ ((ToleranceToFloor * CharacterScaleZ) / UKismetMathLibrary::DegCos(WalkableAngle));
	const FVector& TraceEnd = LowerSphereCenter - DownTraceDistance * CharacterUpVector;

	const FCollisionShape& TraceShape = FCollisionShape::MakeSphere(CapsuleRadius);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Character);
	FHitResult HitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(Character, HitResult, LowerSphereCenter, TraceEnd, FQuat::Identity,
		TraceShape, Character->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceParams);
	
	return Character->GetCharacterMovement()->IsWalkable(HitResult);
}

FVector UVSActorLibrary::GetCharacterRootLocation(const ACharacter* Character, const float VerticalOffset, float UnscaledHalfHeightOverride)
{
	if (!Character) return FVector::ZeroVector;
	const float UnscaledHalfHeightToUse = UnscaledHalfHeightOverride <= 0.f ? Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() : UnscaledHalfHeightOverride;
	return Character->GetActorLocation() - (UnscaledHalfHeightToUse * Character->GetActorScale3D().Z - VerticalOffset) * Character->GetActorUpVector();
}