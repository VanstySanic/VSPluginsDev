// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSDesiredBoundWidgetInterface.generated.h"

class UWidget;

UINTERFACE()
class UVSDesiredBoundWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for widgets that can expose extra typed widgets to a controller.
 *
 * Allows one bound widget to contribute additional (Widget, TypeName) pairs.
 */
class VSWIDGETCORE_API IVSDesiredBoundWidgetInterface
{
	GENERATED_BODY()

public:
	/** Native wrapper for calling Blueprint implementation safely. */
	TMap<UWidget*, FName> GetDesiredBoundTypedWidgets_Native() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Binder")
	TMap<UWidget*, FName> GetDesiredBoundTypedWidgets() const;
};
