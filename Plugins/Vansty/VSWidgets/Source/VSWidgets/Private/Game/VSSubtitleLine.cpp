// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgets/Public/Game/VSSubtitleLine.h"
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
	FString TextToDisplay = !SubtitleParams.SpeakerName.IsEmpty() ?
		(!SubtitleParams.SpeakerNameTextStyleRowName.ToString().IsEmpty() ? ("<" + SubtitleParams.SpeakerNameTextStyleRowName.ToString() + ">" + SubtitleParams.SpeakerName.ToString() + "</>: ") : SubtitleParams.SpeakerName.ToString())
		: FString();
	TextToDisplay += SubtitleParams.SubtitleText.ToString();
	SubtitleLine->RichTextBlock_Subtitle->SetText(FText::FromString(TextToDisplay));
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
