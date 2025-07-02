// Copyright VanstySanic. All Rights Reserved.


#include "Libraries/VSActorLibrary.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"

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
