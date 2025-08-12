// Copyright VanstySanic. All Rights Reserved.

#include "Nodes/VSAnimNode_BlendListByGameplayTag.h"

int32 FVSAnimNode_BlendListByGameplayTag::GetActiveChildIndex()
{
	const FGameplayTag& CurrentActiveTag = GetActiveTag();
	return CurrentActiveTag.IsValid() ? GetTags().Find(CurrentActiveTag) + 1 : 0;
}

const FGameplayTag& FVSAnimNode_BlendListByGameplayTag::GetActiveTag() const
{
	return GET_ANIM_NODE_DATA(FGameplayTag, ActiveTag);
}

const TArray<FGameplayTag>& FVSAnimNode_BlendListByGameplayTag::GetTags() const
{
	return GET_ANIM_NODE_DATA(TArray<FGameplayTag>, Tags);
}

#if WITH_EDITOR
void FVSAnimNode_BlendListByGameplayTag::RefreshPosePins()
{
	const int32 Difference = BlendPose.Num() - GetTags().Num() - 1;
	if (Difference == 0)
	{
		return;
	}
	
	if (Difference > 0)
	{
		for (int32 i = Difference; i > 0; i--)
		{
			RemovePose(BlendPose.Num() - 1);
		}
	}
	else
	{
		for (int32 i = Difference; i < 0; i++)
		{
			AddPose();
		}
	}
}
#endif
