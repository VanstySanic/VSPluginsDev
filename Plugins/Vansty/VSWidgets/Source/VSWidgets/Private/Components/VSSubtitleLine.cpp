// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgets/Public/Components/VSSubtitleLine.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"

UVSSubtitleLine::UVSSubtitleLine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSSubtitleLine* UVSSubtitleLine::CreateAndDisplay(UPanelWidget* ParentPanelWidget, TSubclassOf<UVSSubtitleLine> Class, const FVSWidgetSubtitleParams& SubtitleParams)
{
	if (!ParentPanelWidget || !Class) return nullptr;

	UVSSubtitleLine* SubtitleLine = CreateWidget<UVSSubtitleLine>(ParentPanelWidget, Class);
	if (!SubtitleLine) return nullptr;

	SubtitleLine->SetRenderOpacity(0.f);
	SubtitleLine->AlphaDurationBlend = FVSAlphaDurationBlend(SubtitleParams.AlphaDurationBlendArgs);
	SubtitleLine->AlphaDurationBlend.AlphaDurationBlendArgs.Duration = SubtitleParams.SourceVoice ? SubtitleParams.SourceVoice->GetDuration() : SubtitleParams.AlphaDurationBlendArgs.Duration;
	SubtitleParams.SubtitleText.ApplyToRichTextBlock(SubtitleLine->RichTextBlock_Subtitle);
	SubtitleLine->RichTextBlock_Subtitle->SetText(SubtitleParams.ToText());
	ParentPanelWidget->AddChild(SubtitleLine);

	return SubtitleLine;
}

void UVSSubtitleLine::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AlphaDurationBlend.Update(InDeltaTime);
	OnAlphaUpdated(AlphaDurationBlend.GetAlpha());
	if (AlphaDurationBlend.HasFinished())
	{
		RemoveFromParent();
	}
}

void UVSSubtitleLine::OnAlphaUpdated_Implementation(const float Alpha)
{
	SetRenderOpacity(AlphaDurationBlend.GetAlpha());
}
