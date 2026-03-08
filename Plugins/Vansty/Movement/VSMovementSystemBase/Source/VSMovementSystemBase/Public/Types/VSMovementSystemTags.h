// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
// #include "VSMovementSystemTags.generated.h"

namespace VS
{
	namespace Movement
	{
		namespace Mode
		{
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Walking);
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(NavWalking);
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Falling);
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Swimming);
			VSMOVEMENTSYSTEMBASE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flying);
		}
	}
}