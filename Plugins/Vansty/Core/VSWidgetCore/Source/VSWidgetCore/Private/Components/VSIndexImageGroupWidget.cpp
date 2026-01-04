// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSIndexImageGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

UVSIndexImageGroupWidget::UVSIndexImageGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSIndexImageGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime() || !bDifferRefreshment)
	{
		RefreshIndexImages();
	}
#elif
	if (!bDifferRefreshment)
	{
		RefreshIndexImages();
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

#if WITH_EDITORONLY_DATA
	if (IsDesignTime())
	{
		SetSelectedIndex(PreviewIndex);
	}
#endif
}

void UVSIndexImageGroupWidget::SetSelectedIndex(int32 Index)
{
	const int32 RealImageNum = ImagesPrivate.Num();

#if WITH_EDITORONLY_DATA
	PreviewIndex = Index;
#endif
	
	CurrentIndex = FMath::Clamp(Index, INDEX_NONE, RealImageNum);

	CurrentIndex = Index;
	for (int i = 0; i < RealImageNum; ++i)
	{
		if (ImagesPrivate[i])
		{
			ImagesPrivate[i]->SetBrush(i == CurrentIndex ? SelectedBrush : UnselectedBrush);
		}	
	}
}
