// Copyright VanstySanic. All Rights Reserved.

#include "Nodes/VSAnimGraphNode_BlendListByGameplayTag.h"
#include "GameplayTagsManager.h"
#include "Nodes/VSAnimNode_BlendListByGameplayTag.h"

#define LOCTEXT_NAMESPACE "VSAnimGraphNode_BlendListByGameplayTag"

UVSAnimGraphNode_BlendListByGameplayTag::UVSAnimGraphNode_BlendListByGameplayTag()
{
	Node.AddPose();
}

void UVSAnimGraphNode_BlendListByGameplayTag::PostEditChangeProperty(FPropertyChangedEvent& ChangedEvent)
{
	if (ChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FVSAnimNode_BlendListByGameplayTag, Tags))
	{
		ReconstructNode();
	}

	Super::PostEditChangeProperty(ChangedEvent);
}

FText UVSAnimGraphNode_BlendListByGameplayTag::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Title", "Blend Poses by Gameplay Tag");
}

FText UVSAnimGraphNode_BlendListByGameplayTag::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Tooltip", "Blend list (by gameplay tag)");
}

void UVSAnimGraphNode_BlendListByGameplayTag::ReconstructNode()
{
	/** Must disable orphan pin saving otherwise the invalid blendtime pin will not be removed automatically.  */
	const bool bCurDisableOrphanSaving = bDisableOrphanPinSaving;
	bDisableOrphanPinSaving = true;
	Super::ReconstructNode();
	bDisableOrphanPinSaving = bCurDisableOrphanSaving;
}

void UVSAnimGraphNode_BlendListByGameplayTag::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& PreviousPins)
{
	Node.RefreshPosePins();
	Super::ReallocatePinsDuringReconstruction(PreviousPins);
}

void UVSAnimGraphNode_BlendListByGameplayTag::CustomizePinData(UEdGraphPin* Pin, const FName SourcePropertyName, const int32 PinIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, PinIndex);

	bool bBlendPosePin;
	bool bBlendTimePin;
	GetBlendPinProperties(Pin, bBlendPosePin, bBlendTimePin);

	if (!bBlendPosePin && !bBlendTimePin) return;

	static TFunction<FName(const FGameplayTag&)> GetSimpleTagName = [] (const FGameplayTag& InTag)
	{
		TSharedPtr<FGameplayTagNode> TagNode = UGameplayTagsManager::Get().FindTagNode(InTag);
		return TagNode.IsValid() ? TagNode->GetSimpleTagName() : NAME_None;
	};
	
	Pin->PinFriendlyName = PinIndex <= 0
		? LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Default", "Default")
		: (PinIndex > Node.Tags.Num()
			? LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Invalid", "Invalid")
			: FText::AsCultureInvariant(GetSimpleTagName(Node.Tags[PinIndex - 1]).ToString()));

	if (bBlendPosePin)
	{
		static const FTextFormat BlendPosePinFormat{LOCTEXT("AnimGraphNode_BlendListByGameplayTag_Pose", "{PinName} Pose")};
		Pin->PinFriendlyName = FText::Format(BlendPosePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
	else if (bBlendTimePin)
	{
		static const FTextFormat BlendTimePinFormat{LOCTEXT("AnimGraphNode_BlendListByGameplayTag_BlendTime", "{PinName} Blend Time")};
		Pin->PinFriendlyName = FText::Format(BlendTimePinFormat, {{FString{TEXTVIEW("PinName")}, Pin->PinFriendlyName}});
	}
}

void UVSAnimGraphNode_BlendListByGameplayTag::GetBlendPinProperties(const UEdGraphPin* Pin, bool& bBlendPosePin, bool& bBlendTimePin)
{
	const FString& PinFullName = Pin->PinName.ToString();
	const int32 SeparatorIndex= PinFullName.Find(TEXTVIEW("_"), ESearchCase::CaseSensitive);

	if (SeparatorIndex <= 0)
	{
		bBlendPosePin = false;
		bBlendTimePin = false;
		return;
	}

	const FString& PinName = PinFullName.Left(SeparatorIndex);
	bBlendPosePin = PinName == TEXTVIEW("BlendPose");
	bBlendTimePin = PinName == TEXTVIEW("BlendTime");
}

#undef LOCTEXT_NAMESPACE
