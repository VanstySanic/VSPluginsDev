// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSInteractiveFeature.h"
#include "Types/VSGameplayTypes.h"
#include "VSInteractiveOutlineFeature.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.Interactive.Outline")
class VSINTERACTSYSTEM_API UVSInteractiveOutlineFeature : public UVSInteractiveFeature
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	virtual void OnFeatureDeactivated_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;

private:
	void SetShouldShowOutline(bool bShouldShow);
	bool GetShouldShowOutline() const { return bShouldShowOutline; }
	void ShowOutlineImmediately();
	void HideOutlineImmediately();
	
	UFUNCTION()
	void OnInspectionStart(UVSInteractFeatureAgent* SourceAgent);

	UFUNCTION()
	void OnInspectionEnd(UVSInteractFeatureAgent* SourceAgent);

	// UFUNCTION()
	// void OnInteractionStart(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName);
	
public:
	// /** Whether to override the post process materials. If no in-bound volume found, will always override.  */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	// bool bOverridePostProcessMaterials = true;
	//
	// /** Post process to override.*/
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	// FWeightedBlendables PostProcessMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	bool bOverrideCustomDepthStencilValue = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive", meta = (EditCondition = "bOverrideCustomDepthStencilValue"))
	int32 CustomDepthStencilValue = 1;

	/** Meshes that matches the query will be outlined. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	FVSSceneComponentQuery MeshComponentQuery;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	// bool bOutlineWeightBlendIn = true;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	// bool bOutlineWeightBlendOut = true;
	//
	// /** Any interaction will occur. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	// bool bOutlineHideImmediatelyWithInteraction = true;
	//
	// /** Used to blend the widget opacity. */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive", meta = (EditCondition = "bOutlineWeightBlendIn || bOutlineWeightBlendOut"))
	// FAlphaBlendArgs WeightAlphaBlendArgs;
	
private:
	struct FOutlineMeshData
	{
		int32 OriginalCustomDepthStencilValue = 0;
		// TObjectPtr<APostProcessVolume> PostProcessVolume = nullptr;
		// FWeightedBlendables PostProcessMaterials;
	};
	
	TMap<TWeakObjectPtr<UMeshComponent>, FOutlineMeshData> OutlineMeshDataMap;

	bool bShouldShowOutline = false;
	float OutlineWeightBlendTime = 0.f;
};
