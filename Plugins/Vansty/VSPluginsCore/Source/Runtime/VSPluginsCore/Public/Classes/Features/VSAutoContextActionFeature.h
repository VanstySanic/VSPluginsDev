// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSObjectFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSAutoContextActionFeature.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UVSGameplayTagController;
class UInputMappingContext;

/**
 * 
 */
UCLASS(DisplayName = "Feature.Gameplay.AutoContextAction")
class VSPLUGINSCORE_API UVSAutoContextActionFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Input")
	UVSGameplayTagController* GetGameplayTagController() const { return GameplayTagControllerPrivate.Get(); }

protected:

private:
	UFUNCTION()
	void OnGameplayTagEventNotified(const FGameplayTag& TagEvent);
	
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TMap<TObjectPtr<UInputMappingContext>, FVSAutoContextOptions> QueriedContexts;

private:
	TWeakObjectPtr<UVSGameplayTagController> GameplayTagControllerPrivate;
	TWeakObjectPtr<APlayerController> PlayerControllerPrivate;
	TWeakObjectPtr<ULocalPlayer> LocalPlayerPrivate;
	TWeakObjectPtr<UEnhancedInputComponent> EnhancedInputComponentPrivate;
	TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputLocalPlayerSubsystemPrivate;
};
