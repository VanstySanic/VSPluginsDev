// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemConfig.h"

#include "VSSettingSubsystem.h"
#include "Items/VSSettingItemAgent.h"
#include "Types/Math/VSArray.h"

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

#if WITH_EDITOR
void UVSSettingSystemConfig::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingSystemConfig, SettingItemAgentClasses))
	{
		if (SettingItemAgentClasses != EditorSettingItemAgentClasses)
		{
			if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
			{
				const TArray<TSoftClassPtr<UVSSettingItemAgent>>& Differences = FVSArray::GetArrayDifference(SettingItemAgentClasses, EditorSettingItemAgentClasses);
				TArray<TSoftClassPtr<UVSSettingItemAgent>> AgentClassesToAdd;
				TArray<TSoftClassPtr<UVSSettingItemAgent>> AgentClassesToRemove;
				for (const TSoftClassPtr<UVSSettingItemAgent>& Difference : Differences)
				{
					if (!IsValid(Difference.LoadSynchronous())) continue;
					
					if (SettingItemAgentClasses.Contains(Difference))
					{
						AgentClassesToAdd.Add(Difference);
					}
					else
					{
						AgentClassesToRemove.Add(Difference);
					}
				}

				SettingSubsystem->RemoveEditorDirectSettingItemAgents(AgentClassesToRemove);
				SettingSubsystem->AddDirectSettingItemAgentClasses(AgentClassesToAdd);
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
