// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "Mediator/VSSettingItemWidgetController.h"

UVSSettingItem* IVSSettingItemWidgetMediatorInterface::GetSettingItem_Native() const
{
	if (UObject* Object = _getUObject())
	{
		return Execute_GetSettingItem(Object);
	}

	return nullptr;
}

UVSSettingItem* IVSSettingItemWidgetMediatorInterface::GetSettingItem_Implementation() const
{
	UObject* Object = _getUObject();
	if (!Object) return nullptr;
	
	if (UVSSettingItemWidgetController* SettingItemWidgetController = Cast<UVSSettingItemWidgetController>(Object))
	{
		return SettingItemWidgetController->GetSettingItem_Native();
	}
	if (UObject* ImpOuter = Cast<UObject>(Object->GetImplementingOuterObject(UVSSettingItemWidgetMediatorInterface::StaticClass())))
	{
		return Execute_GetSettingItem(ImpOuter);
	}
	
	return nullptr;
}
