// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "VSChrMovAnimFeature_WallRunMovement.generated.h"

struct FVSAnimSequenceReference;
class UVSChrMovFeature_WallRunMovement;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feautre.ChrMovAnim.Movement.WallRun", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_WallRunMovement : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasValidWallRunStartAnim() const;
	
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasValidWallRunEndAnim() const;
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Node Events")
	void SetupWallRunStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Node Events")
	void UpdateWallRunStartAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Node Events")
	void UpdateWallRunCycleAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Node Events")
	void SetupWallRunEndAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Node Events")
	void UpdateWallRunEndAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
private:
	FVSAnimSequenceReference* GetStartAnimSequenceReferencePtr() const;
	UAnimSequenceBase* GetCycleAnimSequence() const;
	FVSAnimSequenceReference* GetEndAnimSequenceReferencePtr() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_WallRunMovement> ChrMovFeature_WallRunMovement;
	
private:
	struct FAnimData
	{
		int32 LastActionID = INDEX_NONE;
	} AnimData;
};
