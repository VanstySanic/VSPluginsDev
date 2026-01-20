// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSIndexImageGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

UVSIndexImageGroupWidget::UVSIndexImageGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UnselectedBrush.TintColor = FColor::White;
	SelectedBrush.TintColor = FColor::Cyan;
}

void UVSIndexImageGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (!bDifferRefreshment
#if WITH_EDITOR
		|| IsDesignTime()
#endif
		)
	{
		RefreshIndexImages();
	}
	
#if WITH_EDITORONLY_DATA
	if (IsDesignTime())
	{
		SetSelectedIndex(EditorPreviewIndex);
	}
#endif
}

void UVSIndexImageGroupWidget::RefreshIndexImages()
{
	if (!Panel_Images) return;
	
	Panel_Images->ClearChildren();
	ImagesPrivate.Empty();

	for (int i = 0; i < ImageNum; ++i)
	{
		UImage* Image = WidgetTree->ConstructWidget<UImage>();
		
		Panel_Images->AddChild(Image);
		ImagesPrivate.Add(Image);

		ImageSlotSettings.ApplyToWidget(Image);
	}

	for (int i = 0; i < ImageNum; ++i)
	{
		if (ImagesPrivate[i])
		{
			ImagesPrivate[i]->SetBrush(i == CurrentSelectedIndex ? SelectedBrush : UnselectedBrush);
		}	
	}
}

void UVSIndexImageGroupWidget::SetSelectedIndex(int32 Index)
{
	const int32 RealImageNum = ImagesPrivate.Num();

	CurrentSelectedIndex = FMath::Clamp(Index, INDEX_NONE, RealImageNum);
	
#if WITH_EDITORONLY_DATA
	EditorPreviewIndex = CurrentSelectedIndex;
#endif

	CurrentSelectedIndex = Index;
	for (int i = 0; i < RealImageNum; ++i)
	{
		if (ImagesPrivate[i])
		{
			ImagesPrivate[i]->SetBrush(i == CurrentSelectedIndex ? SelectedBrush : UnselectedBrush);
		}	
	}
}
