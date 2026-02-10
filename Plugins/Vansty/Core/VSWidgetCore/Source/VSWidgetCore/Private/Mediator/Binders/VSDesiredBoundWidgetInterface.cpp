// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/VSDesiredBoundWidgetInterface.h"

// Add default functionality here for any IVSWidgetBinderInterface functions that are not pure virtual.
TMap<UWidget*, FName> IVSDesiredBoundWidgetInterface::GetDesiredBoundTypedWidgets_Native() const
{
	if (UObject* Object = _getUObject())
	{
		return Execute_GetDesiredBoundTypedWidgets(Object);
	}

	return TMap<UWidget*, FName>();
}
