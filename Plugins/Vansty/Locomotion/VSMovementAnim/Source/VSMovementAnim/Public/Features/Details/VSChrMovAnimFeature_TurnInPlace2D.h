// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "VSChrMovAnimFeature_TurnInPlace2D.generated.h"

class UVSChrMovFeature_TurnInPlace2D;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMovAnim.Details.TurnInPlace2D", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_TurnInPlace2D : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Conditions")
	bool HasValidTurnInPlaceAnim() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Node Events")
	void SetupTurnInPlaceAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Node Events")
	void UpdateTurnInPlaceAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

protected:
	virtual void Initialize_Implementation() override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_TurnInPlace2D> ChrMovFeature_TurnInPlace2D;

private:
	struct FAnimData
	{
		int32 LastActionID = INDEX_NONE;
	} AnimData;
};
