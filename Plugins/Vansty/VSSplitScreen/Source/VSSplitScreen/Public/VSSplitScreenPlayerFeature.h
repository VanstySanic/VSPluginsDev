// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSplitScreenTypes.h"
#include "Classes/Features/VSObjectFeature.h"
#include "VSSplitScreenPlayerFeature.generated.h"

class UVSAlphaBlendFeature;
/**
 * Put it in pawn or player state.
 */
UCLASS()
class VSSPLITSCREEN_API UVSSplitScreenPlayerFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	EVSSplitScreenPlayer::Type GetSplitScreenPlayer() const { return SplitScreenPlayer; }

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	FVSSplitScreenPlayerData GetSplitScreenPlayerData() const;
	
protected:

	UFUNCTION()
	void OnPlayerStatePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

private:
	EVSSplitScreenPlayer::Type SplitScreenPlayer = EVSSplitScreenPlayer::None;
};
