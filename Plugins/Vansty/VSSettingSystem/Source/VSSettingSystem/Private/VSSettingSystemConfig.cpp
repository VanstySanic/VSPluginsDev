// Copyright VanstySanic. All Rights Reserved.


#include "VSSettingSystemConfig.h"

UVSSettingSystemConfig::UVSSettingSystemConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
FText UVSSettingSystemConfig::GetSectionText() const
{
	return FText::FromString("VSSettingSystem");
}
#endif