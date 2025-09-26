// Copyright VanstySanic. All Rights Reserved.


#include "VSWidgets/Public/Components/VSNotifyLine.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "VSWidgets/Public/Types/VSWidgetTypes.h"

UVSNotifyLine::UVSNotifyLine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSNotifyLine* UVSNotifyLine::CreateAndDisplay(UPanelWidget* ParentPanelWidget, TSubclassOf<UVSNotifyLine> Class, const FVSWidgetNotifyParams& NotifyParams)
{
	if (!ParentPanelWidget || !Class) return nullptr;

	UVSNotifyLine* NotifyLine = CreateWidget<UVSNotifyLine>(ParentPanelWidget, Class);
	if (!NotifyLine) return nullptr;

	NotifyLine->SetRenderOpacity(0.f);
	NotifyLine->AlphaDurationBlend = FVSAlphaDurationBlend(NotifyParams.AlphaDurationBlendArgs);
	
	if (NotifyLine->Image_Notify)
	{
		NotifyLine->Image_Notify->SetBrush(NotifyParams.NotifyImage);
		NotifyLine->Image_Notify->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	NotifyLine->RichTextBlock_NotifyTitle->SetText(NotifyParams.NotifyTitleText);
	if (NotifyLine->RichTextBlock_NotifyContent)
	{
		NotifyParams.NotifyContentText.ApplyToRichTextBlock(NotifyLine->RichTextBlock_NotifyContent);
	}
	
	ParentPanelWidget->AddChild(NotifyLine);

	return NotifyLine;
}

void UVSNotifyLine::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AlphaDurationBlend.Update(InDeltaTime);
	OnAlphaUpdated(AlphaDurationBlend.GetAlpha());
	if (AlphaDurationBlend.HasFinished())
	{
		RemoveFromParent();
	}
}

void UVSNotifyLine::OnAlphaUpdated_Implementation(const float Alpha)
{
	SetRenderOpacity(AlphaDurationBlend.GetAlpha());
}
