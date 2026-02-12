// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/VSWidgetController.h"
#include "Components/Widget.h"
#include "Mediator/Binders//VSWidgetBinder.h"
#include "Mediator/Binders//VSDesiredBoundWidgetInterface.h"

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

void UVSWidgetController::Register()
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

		/** Initialize widget binders. */
		for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
		{
			if (WidgetBinder)
			{
				WidgetBinder->Initialize();
			}
		}
	}

	/** Rebind existing widgets if possible */
	const auto CopiedTypedBoundWidgets = BoundTypedWidgets;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& TypedWidget : CopiedTypedBoundWidgets)
	{
		UnbindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
		BindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
	}
}

void UVSWidgetController::Unregister()
{
	if (!bIsRegistered) return;

	const auto CopiedTypedBoundWidgets = BoundTypedWidgets;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& TypedWidget : CopiedTypedBoundWidgets)
	{
		UnbindWidget(TypedWidget.Value.Get(), TypedWidget.Key);
	}
	
	if (bHasBeenInitialized)
	{
		/** Uninitialize widget binders. */
		for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
		{
			if (WidgetBinder)
			{
				WidgetBinder->Uninitialize();
			}
		}
		
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
	Unregister();
	Register();
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
	UWidget* PrevWidget = BoundTypedWidgets.FindRef(TypeName).Get();
	if (PrevWidget == Widget) return;
	
	UnbindWidget(PrevWidget, TypeName);
	BoundTypedWidgets.Emplace(TypeName, Widget);

	BindTypedWidget(TypeName, Widget);
	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder)
		{
			WidgetBinder->BindTypedWidget(TypeName, Widget);
		}
	}

	if (Widget->GetClass()->ImplementsInterface(UVSDesiredBoundWidgetInterface::StaticClass()))
	{
		for (const TPair<UWidget*, FName>& DesiredBoundTypedWidget : IVSDesiredBoundWidgetInterface::Execute_GetDesiredBoundTypedWidgets(Widget))
		{
			BindWidget(DesiredBoundTypedWidget.Key, DesiredBoundTypedWidget.Value);
		}
	}
}

void UVSWidgetController::UnbindWidget(UWidget* Widget, const FName TypeName)
{
	if (TypeName.IsNone() || !BoundTypedWidgets.Contains(TypeName)) return;
	
	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder)
		{
			WidgetBinder->UnbindTypedWidget(TypeName, Widget);
		}
	}
	UnbindTypedWidget(TypeName, Widget);

	BoundTypedWidgets.Remove(TypeName);
}

void UVSWidgetController::RebindWidgetByType(const FName TypeName)
{
	if (!BoundTypedWidgets.Contains(TypeName)) return;
	UWidget* Widget = BoundTypedWidgets[TypeName].Get();
	UnbindWidget(Widget, TypeName);
	BindWidget(Widget, TypeName);
}

UWidget* UVSWidgetController::GetBoundTypedWidget(const FName TypeName)
{
	return BoundTypedWidgets.FindRef(TypeName).Get();
}

TMap<FName, UWidget*> UVSWidgetController::GetAllBoundTypedWidgets() const
{
	TMap<FName, UWidget*> Ans;
	for (const TPair<FName, TWeakObjectPtr<UWidget>>& BoundTypedWidget : BoundTypedWidgets)
	{
		Ans.Add(BoundTypedWidget.Key, BoundTypedWidget.Value.Get());
	}
	
	return Ans;
}

void UVSWidgetController::TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
{
	Super::TickObject(DeltaTime, TickType, TickFunction);

	for (UVSWidgetBinder* WidgetBinder : WidgetBinders)
	{
		if (WidgetBinder)
		{
			WidgetBinder->UpdateBinder(DeltaTime);
		}
	}
}

void UVSWidgetController::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
}

void UVSWidgetController::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
}