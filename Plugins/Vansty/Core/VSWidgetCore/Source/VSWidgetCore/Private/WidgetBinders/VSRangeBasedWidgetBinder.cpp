// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSRangeBasedWidgetBinder.h"

UVSRangeBasedWidgetBinder::UVSRangeBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSRangeBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();
}

void UVSRangeBasedWidgetBinder::Uninitialize_Implementation()
{
	Super::Uninitialize_Implementation();
}

void UVSRangeBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);
}

void UVSRangeBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}
