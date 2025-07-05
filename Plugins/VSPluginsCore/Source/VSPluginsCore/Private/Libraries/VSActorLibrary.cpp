// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSActorLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSGameplayLibrary.h"

UVSActorLibrary::UVSActorLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(AActor* Actor)
{
	if (!Actor) return false;
	return (Actor->HasAuthority() || Actor->GetLocalRole() == ROLE_AutonomousProxy);
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

bool UVSActorLibrary::IsCharacterOnWalkableFloor(const ACharacter* Character, const float ToleranceToFloor, const bool bConsiderCollisionOffset)
{
	if (!Character) return false;

	/* Character info. */
	const FVector& CharacterUpVector = Character->GetActorUpVector();
	const float CharacterScaleZ = Character->GetActorScale3D().Z;
	const float WalkableAngle = Character->GetCharacterMovement()->GetWalkableFloorAngle();
	const float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector& LowerSphereCenter = Character->GetActorLocation() - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight_WithoutHemisphere() * CharacterUpVector;

	float DownTraceDistance = (ToleranceToFloor * CharacterScaleZ) / UKismetMathLibrary::DegCos(WalkableAngle) + (bConsiderCollisionOffset ? 2.1f : 0.f);
	const FVector& TraceEnd = LowerSphereCenter - DownTraceDistance * CharacterUpVector;

	const FCollisionShape& TraceShape = FCollisionShape::MakeSphere(CapsuleRadius);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(Character);
	FHitResult HitResult;
	UVSGameplayLibrary::SweepSingleByShapeAndChannels(Character, HitResult, LowerSphereCenter, TraceEnd, FQuat::Identity,
		TraceShape, Character->GetCapsuleComponent()->GetCollisionResponseToChannels(), TraceParams);
	
	return Character->GetCharacterMovement()->IsWalkable(HitResult);
}

FVector UVSActorLibrary::GetCharacterRootLocation(const ACharacter* Character, const float VerticalOffset)
{
	if (!Character) return FVector::ZeroVector;
	return Character->GetActorLocation() - (Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - VerticalOffset) * Character->GetActorUpVector();
}