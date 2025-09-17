// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Groups/CommonWidgetGroupBase.h"

UVSGroupWidget::UVSGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WidgetSlotLayoutParams.Padding.Left = 2.f;
	WidgetSlotLayoutParams.Padding.Right = 2.f;
}

void UVSGroupWidget::NativePreConstruct()
{
	RefreshWidgetGroup();
	if (IsDesignTime() || bRefreshWidgetsByDefault)
	{
		RefreshWidgets();
	}

	Super::NativePreConstruct();
}

void UVSGroupWidget::SetWidgetNum(int32 InWidgetNum)
{
	if (WidgetNum == InWidgetNum || InWidgetNum < 1) return;
	WidgetNum = InWidgetNum;
	RefreshWidgets();
}

TArray<UWidget*> UVSGroupWidget::GetWidgets() const
{
	return Panel ? Panel->GetAllChildren() : TArray<UWidget*>();
}

void UVSGroupWidget::RefreshWidgets()
{
	if (!Panel || !WidgetClass) return;
	
	if (WidgetGroup)
	{
		WidgetGroup->RemoveAll();
	}
	
	for (UWidget* Child : Panel->GetAllChildren())
	{
		if (Child)
		{
			Child->RemoveFromParent();
		}
	}
	Panel->ClearChildren();

	for (int i = 0; i < WidgetNum; i++)
	{
		UWidget* Widget = WidgetTree->ConstructWidget(WidgetClass);
		Panel->AddChild(Widget);
		Widget->SetVisibility(WidgetVisibility);
		WidgetSlotLayoutParams.ApplyToWidget(Widget);
	}

	if (WidgetGroup && Panel)
	{
		WidgetGroup->AddWidgets(Panel->GetAllChildren());
	}
}

void UVSGroupWidget::RefreshWidgetGroup()
{
	if (!Panel || !WidgetClass || !WidgetGroupClass) return;
	
	if (WidgetGroup)
	{
		WidgetGroup->MarkAsGarbage();
		WidgetGroup = nullptr;
	}

	WidgetGroup = NewObject<UCommonWidgetGroupBase>(this, WidgetGroupClass);
	if (WidgetGroup)
	{
		if (Panel)
		{
			WidgetGroup->AddWidgets(Panel->GetAllChildren());
		}
	}
}
