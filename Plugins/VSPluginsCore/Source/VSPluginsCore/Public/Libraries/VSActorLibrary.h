// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VSActorLibrary.generated.h"

class UAbilitySystemComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSActorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameplay", meta = (DefaultToSelf = "Source"))
	static bool IsActorLocalRoleAuthorityOrAutonomous(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UActorComponent* GetActorComponentByName(const AActor* Actor, FName ComponentName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UCameraComponent* GetActiveCameraFromActor(const AActor* Actor);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Actor", meta = (DefaultToSelf = "Actor"))
	static UAbilitySystemComponent* GetAbilitySystemComponentFormActor(AActor* Actor);
};
