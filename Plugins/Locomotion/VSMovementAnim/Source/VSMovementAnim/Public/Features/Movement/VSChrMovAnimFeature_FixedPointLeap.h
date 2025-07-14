// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "VSChrMovAnimFeature_FixedPointLeap.generated.h"

class UVSChrMovFeature_FixedPointLeap;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feautre.ChrMovAnim.Movement.FixedPointLeap", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_FixedPointLeap : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Initialize_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	bool HasValidFixedPointLeapAnim() const;

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void SetupFixedPointLeapAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable, Category = "Node Events")
	void UpdateFixedPointLeapAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_FixedPointLeap> ChrMovFeature_FixedPointLeapMovement;
	
private:
	struct FAnimData
	{
		int32 LastActionID = INDEX_NONE;
	} AnimData;
};
