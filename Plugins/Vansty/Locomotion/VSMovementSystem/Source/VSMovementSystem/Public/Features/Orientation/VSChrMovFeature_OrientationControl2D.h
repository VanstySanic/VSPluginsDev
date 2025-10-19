// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSCharacterMovementTags.h"
#include "Types/VSChrMovOrientationTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_OrientationControl2D.generated.h"

/**
 * Gives convenient features for 2D orientation control.
 * This works for movement modes that just requires simple orientation or automatic management.
 * 2D means the character's local space yaw.
 */
UCLASS(DisplayName = "Feature.ChrMov.Orientation.Control2D")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_OrientationControl2D : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Orientation")
	FVSOrientationControlSettings2D GetOrientationControlSettings2D() const { return MovementData.CurrentSettings; }

protected:
	virtual void BeginPlay_Implementation() override;
	virtual void UpdateMovement_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

private:
	void UpdateTagQueryStates(const FGameplayTag& TagEvent);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orientation")
	FVSOrientationControlSettings2D DefaultSettings;
	
	UPROPERTY(EditAnywhere, Category = "Orientation")
	TMap<FVSOrientationControlSettings2D, FVSGameplayTagEventQuery> TagQueriedSettings;

	UPROPERTY(EditAnywhere, Category = "Orientation")
	FVSGameplayTagEventQuery RefreshQueriedSettingsTagQuery;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orientation")
	FVSGameplayTagEventQuery MovementTagQuery; 

private:
	struct FMovementData
	{
		FVSOrientationControlSettings2D CurrentSettings;
		
		bool bMatchesTagQuery = false;
	} MovementData;
};
