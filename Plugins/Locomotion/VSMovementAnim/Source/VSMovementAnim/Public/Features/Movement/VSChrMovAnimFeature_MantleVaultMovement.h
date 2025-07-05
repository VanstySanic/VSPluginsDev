// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "VSChrMovAnimFeature_MantleVaultMovement.generated.h"

class UVSChrMovFeature_MantleVaultMovement;
struct FAnimUpdateContext;
struct FAnimNodeReference;

/**
 * 
 */
UCLASS(DisplayName = "Feautre.ChrMovAnim.Movement.MantleVault", meta = (BlueprintThreadSafe))
class VSMOVEMENTANIM_API UVSChrMovAnimFeature_MantleVaultMovement : public UVSCharacterMovementAnimFeature
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Initialize_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Node Events")
	bool HasValidMantleVaultAnim() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Node Events")
	void SetupMantleVaultAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Node Events")
	void UpdateMantleVaultAnim(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	TWeakObjectPtr<UVSChrMovFeature_MantleVaultMovement> ChrMovFeature_MantleMovement;
	
private:
	struct FAnimData
	{
		int32 LastMantleActionID = INDEX_NONE;
	} AnimData;
	
};
