// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Animation/Nodes/VSAnimGraphNode_MultiWayBlendWithInterpolation.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "VSPCE_AnimGraphNode_MultiWayBlendWithInterpolation"

FString UVSAnimGraphNode_MultiWayBlendWithInterpolation::GetNodeCategory() const
{
	return TEXT("Animation|Blends");
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("AnimGraphBlendMulti", NSLOCTEXT("AnimGraphNode_MultiWayBlend", "BlendMultiHeader", "BlendMulti"));

			if (Context->Pin != NULL)
			{
				if (Context->Pin->Direction == EGPD_Input)
				{
					FUIAction Action = FUIAction( FExecuteAction::CreateUObject(const_cast<UVSAnimGraphNode_MultiWayBlendWithInterpolation*>(this), &UVSAnimGraphNode_MultiWayBlendWithInterpolation::RemovePinFromBlendNode, const_cast<UEdGraphPin*>(Context->Pin)) );
					Section.AddMenuEntry("RemoveBlendListPin", LOCTEXT("RemoveBlendListPin", "Remove Blend List Pin"), FText::GetEmpty(), FSlateIcon(), Action);
				}
			}
			else
			{
				FUIAction Action = FUIAction( FExecuteAction::CreateUObject(const_cast<UVSAnimGraphNode_MultiWayBlendWithInterpolation*>(this), &UVSAnimGraphNode_MultiWayBlendWithInterpolation::AddPinToBlendNode) );
				Section.AddMenuEntry("AddBlendListPin", LOCTEXT("AddBlendListPin", "Add Blend List Pin"), FText::GetEmpty(), FSlateIcon(), Action);
			}
		}
	}
}

FLinearColor UVSAnimGraphNode_MultiWayBlendWithInterpolation::GetNodeTitleColor() const
{
	return FLinearColor(0.75f, 0.75f, 0.75f);
}

FText UVSAnimGraphNode_MultiWayBlendWithInterpolation::GetTooltipText() const
{
	return LOCTEXT("MultiWayBlendTooltip", "Blend multiple poses together by Alpha, with interpolation.");
}

FText UVSAnimGraphNode_MultiWayBlendWithInterpolation::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Blend", "Blend Multi with Interpolation");
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::AddPinToBlendNode()
{
	FScopedTransaction Transaction(NSLOCTEXT("AnimGraphNode_MultiWayBlend", "AddBlendMultiPin", "AddBlendMultiPin"));
	Modify();

	Node.AddPose();
	ReconstructNode();

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::RemovePinFromBlendNode(UEdGraphPin* Pin)
{
	FScopedTransaction Transaction(NSLOCTEXT("AnimGraphNode_MultiWayBlend", "RemoveBlendMultiPin", "RemoveBlendMultiPin"));
	Modify();

	FProperty* AssociatedProperty;
	int32 ArrayIndex;
	GetPinAssociatedProperty(GetFNodeType(), Pin, /*out*/ AssociatedProperty, /*out*/ ArrayIndex);

	if (ArrayIndex != INDEX_NONE)
	{
		RemovedPinArrayIndex = ArrayIndex;
		Node.RemovePose(ArrayIndex);
		ReconstructNode();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	
	Node.AddPose();
	Node.AddPose();
	ReconstructNode();
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (RemovedPinArrayIndex != INDEX_NONE)
	{
		RemovePinsFromOldPins(OldPins, RemovedPinArrayIndex);
		RemovedPinArrayIndex = INDEX_NONE;
	}
}

void UVSAnimGraphNode_MultiWayBlendWithInterpolation::RemovePinsFromOldPins(TArray<UEdGraphPin*>& OldPins, int32 RemovedArrayIndex)
{
	TArray<FString> RemovedPropertyNames;
	TArray<FName> NewPinNames;

	for (int32 NewPinIndx = 0; NewPinIndx < Pins.Num(); NewPinIndx++)
	{
		NewPinNames.Add(Pins[NewPinIndx]->PinName);
	}

	for (int32 OldPinIdx = 0; OldPinIdx < OldPins.Num(); OldPinIdx++)
	{
		const FName OldPinName = OldPins[OldPinIdx]->PinName;
		if (!NewPinNames.Contains(OldPinName))
		{
			const FString OldPinNameStr = OldPinName.ToString();
			const int32 UnderscoreIndex = OldPinNameStr.Find(TEXT("_"), ESearchCase::CaseSensitive);
			if (UnderscoreIndex != INDEX_NONE)
			{
				FString PropertyName = OldPinNameStr.Left(UnderscoreIndex);
				RemovedPropertyNames.Add(MoveTemp(PropertyName));
			}
		}
	}

	for (int32 PinIdx = 0; PinIdx < OldPins.Num(); PinIdx++)
	{
		const FString OldPinNameStr = OldPins[PinIdx]->PinName.ToString();

		const int32 UnderscoreIndex = OldPinNameStr.Find(TEXT("_"), ESearchCase::CaseSensitive);
		if (UnderscoreIndex != INDEX_NONE)
		{
			const FString PropertyName = OldPinNameStr.Left(UnderscoreIndex);

			if (RemovedPropertyNames.Contains(PropertyName))
			{
				const int32 ArrayIndex = FCString::Atoi(*(OldPinNameStr.Mid(UnderscoreIndex + 1)));
				if (ArrayIndex == RemovedArrayIndex)
				{
					OldPins[PinIdx]->MarkAsGarbage();
					OldPins.RemoveAt(PinIdx);
					--PinIdx;
				}
				else if (ArrayIndex > RemovedArrayIndex)
				{
					OldPins[PinIdx]->PinName = *FString::Printf(TEXT("%s_%d"), *PropertyName, ArrayIndex - 1);
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE