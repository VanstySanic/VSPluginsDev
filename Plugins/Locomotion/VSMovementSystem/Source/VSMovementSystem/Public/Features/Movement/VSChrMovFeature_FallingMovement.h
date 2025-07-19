// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSGameplayTypes.h"
#include "Types/VSMathTypes.h"
#include "VSChrMovFeature_FallingMovement.generated.h"

namespace EVSNetAuthorityMethodExecPolicy
{
	enum Type : int32;
}

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.Falling")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_FallingMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()
	
protected:
	virtual bool CanUpdateMovement_Implementation() const override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "NewVelocity, NetPolicies"))
	void LaunchCharacter(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride = EVSVectorAxes::XYZ, const FVSNetMethodExecutionPolicies& NetPolicies = FVSNetMethodExecutionPolicies());

private:
	void LaunchCharacterInternal(const FVector& NewVelocity, TEnumAsByte<EVSVectorAxes::Type> RelativeAxesToOverride = EVSVectorAxes::XYZ);
	
	UFUNCTION(Server, Reliable)
	void LaunchCharacter_Server(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride, EVSNetAuthorityMethodExecPolicy::Type NetPolicy);
	
	UFUNCTION(NetMulticast, Reliable)
	void LaunchCharacter_Multicast(const FVector& NewVelocity, EVSVectorAxes::Type RelativeAxesToOverride, EVSNetAuthorityMethodExecPolicy::Type NetPolicy);

private:
	struct FMovementData
	{
		int32 CachedJumpCount = 0;
	} MovementData;
};
