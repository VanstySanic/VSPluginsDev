// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSSettingItemEnabledStateControlWidgetBinder.h"
#include "Components/Widget.h"

UVSSettingItemEnabledStateControlWidgetBinder::UVSSettingItemEnabledStateControlWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSSettingItemEnabledStateControlWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshEnabledState();
}

void UVSSettingItemEnabledStateControlWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Item"))
	{
		RefreshEnabledState();
	}
}

void UVSSettingItemEnabledStateControlWidgetBinder::RefreshEnabledState()
{
	if (UWidget* Widget = GetBoundTypedWidget(FName("Item")))
	{
		const bool bEnabledState = GetDesiredEnabledState();
		Widget->SetIsEnabled(bEnabledState);
	}
}

bool UVSSettingItemEnabledStateControlWidgetBinder::GetDesiredEnabledState_Implementation()
{
	return true;
}
