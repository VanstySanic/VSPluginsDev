// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemConfig.h"
#include "VSSettingSubsystem.h"

UVSSettingSystemConfig::UVSSettingSystemConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::Get()
{
	return GetDefault<UVSSettingSystemConfig>();
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::GetSettingSystemConfig_VS()
{
	return GetDefault<UVSSettingSystemConfig>();
}

void UVSSettingSystemConfig::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorSettingItemAgentClasses = SettingItemAgentClasses;
#endif
}

#if WITH_EDITOR
void UVSSettingSystemConfig::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingSystemConfig, SettingItemAgentClasses))
	{
		if (SettingItemAgentClasses != EditorSettingItemAgentClasses)
		{
			if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
			{
				SettingSubsystem->RefreshEditorDirectSettingItemAgents();
			}

			EditorSettingItemAgentClasses = SettingItemAgentClasses;
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FName UVSSettingSystemConfig::GetCategoryName() const
{
	return FName("Engine");
}
