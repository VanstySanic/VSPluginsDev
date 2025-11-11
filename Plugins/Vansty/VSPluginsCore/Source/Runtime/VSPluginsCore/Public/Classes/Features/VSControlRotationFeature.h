// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSObjectFeature.h"
#include "VSControlRotationFeature.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Gameplay.ControlRotation")
class VSPLUGINSCORE_API UVSControlRotationFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Initialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Features")
	FRotator GetControlRotation() const { return ControlRotation; }

	UFUNCTION(BlueprintCallable, Category = "Features")
	void SetController(AController* NewController);
	
private:
	UFUNCTION(Server, Unreliable)
	void SyncControlRotation_Server(const FRotator& Rotation);

	UFUNCTION()
	void OnRep_ReplicatedControlRotation();
	
private:
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedControlRotation")
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

	FRotator ControlRotation = FRotator::ZeroRotator;
	TWeakObjectPtr<AController> Controller = nullptr;
};
