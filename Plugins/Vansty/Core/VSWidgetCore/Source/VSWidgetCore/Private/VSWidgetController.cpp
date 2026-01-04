// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgetController.h"
#include "Components/Widget.h"

UVSWidgetController::UVSWidgetController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSWidgetController::BeginDestroy()
{
	if (IsRegistered())
	{
		Unregister();
	}
	
	Super::BeginDestroy();
}

void UVSWidgetController::Register(const TMap<FName, UWidget*>& TypedWidgetsToBind)
{
	if (bIsRegistered) return;
	bIsRegistered = true;

	if (!IsTickFunctionRegistered())
	{
		RegisterTickFunction();
	}
	
	if (!bHasBeenInitialized)
	{
		Initialize();
		bHasBeenInitialized = true;
	}

	/** Rebind existing widgets if possible */
	const auto CopiedTypedBoundWidgets = TypedBoundWidgets;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& TypedWidget : CopiedTypedBoundWidgets)
	{
		UnbindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
		BindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
	}
	
	for (const TPair<FName, UWidget*>& TypedWidget : TypedWidgetsToBind)
	{
		BindWidget(TypedWidget.Value, TypedWidget.Key);
	}
}

void UVSWidgetController::Unregister()
{
	if (!bIsRegistered) return;

	const auto CopiedTypedBoundWidgets = TypedBoundWidgets;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& TypedWidget : CopiedTypedBoundWidgets)
	{
		UnbindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
	}
	
	if (bHasBeenInitialized)
	{
		Uninitialize();
		bHasBeenInitialized = false;
	}
	
	if (IsTickFunctionRegistered())
	{
		UnregisterTickFunction();
	}
	
	bIsRegistered = false;
}

void UVSWidgetController::Reregister()
{
	if (!IsRegistered()) return;
	TMap<FName, UWidget*> CopiedTypedBoundWidgets;;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& TypedBoundWidget : TypedBoundWidgets)
	{
		CopiedTypedBoundWidgets.Emplace(TypedBoundWidget.Key, TypedBoundWidget.Value.Get());
	}
	Unregister();
	Register(CopiedTypedBoundWidgets);
}

void UVSWidgetController::Initialize_Implementation()
{
	
}

void UVSWidgetController::Uninitialize_Implementation()
{
	
}

void UVSWidgetController::BindWidget(UWidget* Widget, const FName TypeName)
{
	if (!Widget || TypeName.IsNone()) return;
	UWidget* PrevWidget = TypedBoundWidgets.FindRef(TypeName).Get();
	if (PrevWidget == Widget) return;
	
	UnbindWidget(PrevWidget, TypeName);
	TypedBoundWidgets.Emplace(TypeName, Widget);
	BindTypedWidget(TypeName, Widget);
}

void UVSWidgetController::UnbindWidget(UWidget* Widget, const FName TypeName)
{
	if (TypeName.IsNone() || !TypedBoundWidgets.Contains(TypeName)) return;
	UnbindTypedWidget(TypeName, Widget);
	TypedBoundWidgets.Remove(TypeName);
}

UWidget* UVSWidgetController::GetTypedBoundWidget(const FName TypeName)
{
	return TypedBoundWidgets.FindRef(TypeName).Get();
}

void UVSWidgetController::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
}

void UVSWidgetController::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
}