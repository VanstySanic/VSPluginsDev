// Copyright VanstySanic. All Rights Reserved.

#include "Libraries/VSActorLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Classes/Framework/VSObjectFeature.h"
#include "Classes/Framework/VSObjectFeatureComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
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
		GameplayTagController = FindFeatureByClassFromActor<UVSGameplayTagController>(Actor);
	}
	return GameplayTagController;
}

UVSObjectFeature* UVSActorLibrary::GetFeatureByClassFromActor(AActor* Actor, TSubclassOf<UVSObjectFeature> Class)
{
	if (!Actor || !Class) return nullptr;
	TArray<UActorComponent*> Components;
	Actor->GetComponents(UVSObjectFeature::StaticClass(), Components);
	for (UActorComponent* Component : Components)
	{
		if (UVSObjectFeatureComponent* FeatureComponent = Cast<UVSObjectFeatureComponent>(Component))
		{
			if (UVSObjectFeature* Feature = FeatureComponent->GetSubFeatureByClass(Class))
			{
				return Feature;
			}
		}
	}

	for (FProperty* Property = Actor->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
		if (!ObjectProperty) continue;
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Actor);
		if (!ValuePtr) continue;
		UObject* Value = ObjectProperty->GetPropertyValue(ValuePtr);
		if (!Value) continue;
		if (UVSObjectFeature* Feature = Cast<UVSObjectFeature>(Value))
		{
			if (Value->IsA(Class))
			{
				return Feature;
			}
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByClass(Class))
			{
				return SubFeature;
			}
		}
	}
	
	return nullptr;
}

UVSObjectFeature* UVSActorLibrary::GetFeatureByNameFromActor(AActor* Actor, FName Name)
{
	if (!Actor || Name.IsNone()) return nullptr;
	TArray<UActorComponent*> Components;
	Actor->GetComponents(UVSObjectFeature::StaticClass(), Components);
	for (UActorComponent* Component : Components)
	{
		if (UVSObjectFeatureComponent* FeatureComponent = Cast<UVSObjectFeatureComponent>(Component))
		{
			if (UVSObjectFeature* Feature = FeatureComponent->GetSubFeatureByName(Name))
			{
				return Feature;
			}
		}
	}

	for (FProperty* Property = Actor->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property);
		if (!ObjectProperty) continue;
		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Actor);
		if (!ValuePtr) continue;
		UObject* Value = ObjectProperty->GetPropertyValue(ValuePtr);
		if (!Value) continue;
		if (UVSObjectFeature* Feature = Cast<UVSObjectFeature>(Value))
		{
			if (Feature->FeatureName == Name)
			{
				return Feature;
			}
			if (UVSObjectFeature* SubFeature = Feature->GetSubFeatureByName(Name))
			{
				return SubFeature;
			}
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

	float DownTraceDistance = (bConsiderCollisionOffset ? 2.56f : 0.f) + (ToleranceToFloor * CharacterScaleZ) / UKismetMathLibrary::DegCos(WalkableAngle);
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