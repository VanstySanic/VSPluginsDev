// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_MultiWayBlend.h"
#include "Types/Animation/Nodes/VSAnimNode_MultiWayBlendWithInterpolation.h"
#include "VSAnimGraphNode_MultiWayBlendWithInterpolation.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCOREEDITOR_API UVSAnimGraphNode_MultiWayBlendWithInterpolation : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:
	virtual void AddPinToBlendNode();
	virtual void RemovePinFromBlendNode(UEdGraphPin* Pin);
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;

	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostPlacedNewNode() override;

	virtual FString GetNodeCategory() const override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

public:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FVSAnimNode_MultiWayBlendWithInterpolation Node;
	
private:
	void RemovePinsFromOldPins(TArray<UEdGraphPin*>& OldPins, int32 RemovedArrayIndex);
	int32 RemovedPinArrayIndex;
};
