// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/AnimGraph/Public/AnimGraphNode_BlendListBase.h"
#include "Types/Animation/Nodes/VSAnimNode_BlendListByGameplayTag.h"
#include "VSAnimGraphNode_BlendListByGameplayTag.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCOREEDITOR_API UVSAnimGraphNode_BlendListByGameplayTag : public UAnimGraphNode_BlendListBase
{
	GENERATED_BODY()

public:
	UVSAnimGraphNode_BlendListByGameplayTag();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual void ReconstructNode() override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins) override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 PinIndex) const override;

protected:
	static void GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSAnimNode_BlendListByGameplayTag Node;
};
