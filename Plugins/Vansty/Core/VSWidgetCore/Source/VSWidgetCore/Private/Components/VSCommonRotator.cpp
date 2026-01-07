// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonRotator.h"
#include "Components/VSIndexImageGroupWidget.h"

UVSCommonRotator::UVSCommonRotator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bNavigationAllowWrapping(true)
{
	
}

void UVSCommonRotator::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (DefaultOptions.Num())
	{
		PopulateTextLabels(DefaultOptions);
	}

#if WITH_EDITORONLY_DATA
	SetSelectedItem(EditorPreviewIndex);
#endif

	if (IndexImageGroup)
	{
		IndexImageGroup->ImageNum = TextLabels.Num();
		IndexImageGroup->RefreshIndexImages();
		IndexImageGroup->SetSelectedIndex(GetSelectedIndex());
	}
}

bool UVSCommonRotator::Initialize()
{
	if (!Super::Initialize()) return false;

	if (Button_Prev)
	{
		Button_Prev->OnClicked.AddDynamic(this, &UVSCommonRotator::OnButtonPrevClicked);
	}

	if (Button_Next)
	{
		Button_Next->OnClicked.AddDynamic(this, &UVSCommonRotator::OnButtonNextClicked);
	}

	return true;
}

FNavigationReply UVSCommonRotator::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	switch (InNavigationEvent.GetNavigationType())
	{
	case EUINavigation::Left:
	case EUINavigation::Right:
		{
			if (bNavigationAllowWrapping)
			{
				return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
			}

			/**/
			const int32 CurrentIndex = GetSelectedIndex();
			const int32 DesiredIndex = FMath::Clamp(CurrentIndex + (InNavigationEvent.GetNavigationType() == EUINavigation::Left ? -1 : 1), 0, TextLabels.Num() - 1);
			if (CurrentIndex != DesiredIndex)
			{
				return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
			}
		}
		break;
		
	default:
		return InDefaultReply;
	}

	return InDefaultReply;
}

void UVSCommonRotator::SetSelectedItem(int32 InValue)
{
	Super::SetSelectedItem(InValue);
	
#if WITH_EDITORONLY_DATA
	EditorPreviewIndex = GetSelectedIndex();
#endif
	
	if (IndexImageGroup)
	{
		IndexImageGroup->SetSelectedIndex(InValue);
	}
}

void UVSCommonRotator::OnButtonPrevClicked()
{
	if (bNavigationAllowWrapping)
	{
		ShiftTextLeft();
	}
	else
	{
		const int32 CurrentIndex = GetSelectedIndex();
		const int32 DesiredIndex = FMath::Clamp(CurrentIndex - 1, 0, TextLabels.Num() - 1);
		if (CurrentIndex != DesiredIndex)
		{
			ShiftTextLeft();
		}
	}
}

void UVSCommonRotator::OnButtonNextClicked()
{
	if (bNavigationAllowWrapping)
	{
		ShiftTextRight();
	}
	else
	{
		const int32 CurrentIndex = GetSelectedIndex();
		const int32 DesiredIndex = FMath::Clamp(CurrentIndex + 1, 0, TextLabels.Num() - 1);
		if (CurrentIndex != DesiredIndex)
		{
			ShiftTextRight();
		}
	}
}
