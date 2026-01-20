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
 * 
 */
class VSWIDGETCORE_API IVSDesiredBoundWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Binder")
	TMap<UWidget*, FName> GetDesiredBoundTypedWidgets() const;
};
