// Copyright VanstySanic. All Rights Reserved.

#include "Misc/VSImageIndexWidgetGroup.h"
#include "Components/Image.h"

UVSImageIndexWidgetGroup::UVSImageIndexWidgetGroup()
{
}

TSubclassOf<UWidget> UVSImageIndexWidgetGroup::GetWidgetType() const
{
	return UImage::StaticClass();
}

void UVSImageIndexWidgetGroup::SetIndex(int32 InIndex)
{
	if (InIndex == Index) return;
	if (!Images.IsValidIndex(InIndex) && InIndex != INDEX_NONE) return;

	const int32 PrevIndex = Index;
	Index = InIndex;

	/** Update images. */
	if (Images.IsValidIndex(PrevIndex))
	{
		Images[PrevIndex]->SetBrush(UnindexedBrush);
	}
	if (Images.IsValidIndex(Index))
	{
		Images[Index]->SetBrush(IndexedBrush);
	}
}

void UVSImageIndexWidgetGroup::SetIndexedBrush(const FSlateBrush& InBrush)
{
	IndexedBrush = InBrush;
	if (Images.IsValidIndex(Index))
	{
		Images[Index]->SetBrush(IndexedBrush);
	}
}

void UVSImageIndexWidgetGroup::SetUnindexedBrush(const FSlateBrush& InBrush)
{
	UnindexedBrush = InBrush;
	for (TWeakObjectPtr<UImage> Image : Images)
	{
		if (Image.IsValid())
		{
			if (!Images.IsValidIndex(Index) || Images[Index] != Image.Get())
			{
				Image->SetBrush(UnindexedBrush);
			}
		}
	}
}

void UVSImageIndexWidgetGroup::OnWidgetAdded(UWidget* NewWidget)
{
	UImage* Image = Cast<UImage>(NewWidget);
	if (!Image || Images.Contains(Image)) return;
	Images.Add(Image);
	
	if (Images.Num() == Index + 1)
	{
		Image->SetBrush(IndexedBrush);
	}
	else
	{
		Image->SetBrush(UnindexedBrush);
	}
}

void UVSImageIndexWidgetGroup::OnWidgetRemoved(UWidget* OldWidget)
{
	UImage* Image = Cast<UImage>(OldWidget);
	if (!Image || !Images.Contains(Image)) return;
	Images.RemoveSingle(Image);

	if (Images.Num() <= Index)
	{
		SetIndex(INDEX_NONE);
	}
}

void UVSImageIndexWidgetGroup::OnRemoveAll()
{
	Images.Empty();
	SetIndex(INDEX_NONE);
}