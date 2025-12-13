// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSActorLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

UVSActorLibrary::UVSActorLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSActorLibrary::IsActorNetLocal(const AActor* Actor)
{
	if (!Actor) return false;

	if (Actor->HasLocalNetOwner()) return true;
	if (Actor->GetNetMode() == NM_Standalone || Actor->GetLocalRole() == ROLE_AutonomousProxy) return true;
	if (Actor->GetLocalRole() == ROLE_Authority && Actor->GetRemoteRole() == ROLE_AutonomousProxy) return true;
	
	return false;
}

bool UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(const AActor* Actor)
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

	AActor* NewActor = World->SpawnActor<AActor>(Actor->GetClass(), SpawnTransform, Params);
    
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

APawn* UVSActorLibrary::GetPawnFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn;
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		return PlayerState->GetPawn();
	}
	if (AController* Controller = Cast<AController>(Actor))
	{
		return Controller->GetPawn();
	}
	
	return GetPawnFromActor(Actor->GetOwner());
}

APlayerState* UVSActorLibrary::GetPlayerStateFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		return PlayerState;
	}
	if (AController* Controller = Cast<AController>(Actor))
	{
		return Controller->PlayerState;
	}
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetPlayerState();
	}
	
	return GetPlayerStateFromActor(Actor->GetOwner());
}

AController* UVSActorLibrary::GetControllerFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	if (AController* Controller = Cast<AController>(Actor))
	{
		return Controller;
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
	{
		return PlayerState->GetOwningController();
	}
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetController();
	}
	
	return GetControllerFromActor(Actor->GetOwner());
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

// UAbilitySystemComponent* UVSActorLibrary::GetAbilitySystemComponentFormActor(AActor* Actor)
// {
// 	if (!Actor) return nullptr;
// 	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Actor))
// 	{
// 		if (UAbilitySystemComponent* AbilitySystemComponent = Interface->GetAbilitySystemComponent())
// 		{
// 			return AbilitySystemComponent;
// 		}
// 	}
//
// 	if (UAbilitySystemComponent* AbilitySystemComponent = Actor->FindComponentByClass<UAbilitySystemComponent>())
// 	{
// 		return AbilitySystemComponent;
// 	}
// 	
// 	return nullptr;
// }