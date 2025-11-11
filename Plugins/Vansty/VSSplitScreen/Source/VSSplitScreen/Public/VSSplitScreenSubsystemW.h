// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSplitScreenTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "VSSplitScreenSubsystemW.generated.h"

class UVSObjectFeature;
class UVSAlphaBlendFeature;

/**
 * 
 */
UCLASS()
class VSSPLITSCREEN_API UVSSplitScreenSubsystemW : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UVSSplitScreenSubsystemW();
	static UVSSplitScreenSubsystemW* Get(const UObject* WorldContext);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Manual call in GameState's StartPlay or similar position. */
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void InitializeSplitScreen();

	/** Manual call in GameState's endplay. */
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void DeinitializeSplitScreen();

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	void SetPlayerState(EVSSplitScreenPlayer::Type Player, APlayerState* PlayerState);
	
	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	FVSSplitScreenPlayerData GetSplitScreenPlayerData(EVSSplitScreenPlayer::Type Player) const;

	UFUNCTION(BlueprintCallable, Category = "Split Screen")
	float GetPlayerLayoutRatio() const { return PlayerLayoutRatio; }
	
	UFUNCTION(BlueprintCallable, Category = "Split Screen", meta = (AutoCreateRefTerm = "AlphaBlendArgs"))
	void SetPlayerLayoutRatioWithBlend(float InRatio = 0.5f, const FAlphaBlendArgs& AlphaBlendArgs = FAlphaBlendArgs());

	UFUNCTION(BlueprintCallable, Category = "Split Screen", meta = (AutoCreateRefTerm = "TransitionParams"))
	void SetPlayerViewTargetWithBlend(EVSSplitScreenPlayer::Type Player, AActor* ViewTarget, const FViewTargetTransitionParams& TransitionParams = FViewTargetTransitionParams());

	
private:
	void SetPlayerLayoutRatioInternal(float InRatio = 0.5f);
	FSplitscreenData GetTwoPlayerVerticalSplitScreenData();
	void SetTwoPlayerVerticalSplitScreenData(const FSplitscreenData& Data);

	UFUNCTION()
	void OnPlayerLayoutRatioAlphaBlendFeatureUpdated(UVSAlphaBlendFeature* Feature, float Alpha, float UpdatedTime);

protected:
	UPROPERTY()
	TObjectPtr<UVSAlphaBlendFeature> PlayerLayoutRatioAlphaBlendFeature;
	
private:
	/** [PlayerOne, PlayerTwo] */
	TMap<EVSSplitScreenPlayer::Type, FVSSplitScreenPlayerData> PlayerDatas;

	UPROPERTY()
	TObjectPtr<APlayerController> LocalController_PlayerID1;
	
	float PlayerLayoutRatio = 0.5f;
	float DesiredPlayerLayoutRatio = 0.5f;
	float BlendStartPlayerLayoutRatio = 0.5f;
	FDelegateHandle OnWorldBeginTearDownDelegateHandle;

	FSplitscreenData* SplitScreenData = nullptr;
};