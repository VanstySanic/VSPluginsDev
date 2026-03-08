// Copyright VanstySanic. All Rights Reserved.


#include "Types/VSMovementSystemTags.h"

namespace VS
{
	namespace Movement
	{
		namespace Mode
		{
			UE_DEFINE_GAMEPLAY_TAG(None, "VS.Movement.Mode.None");
			UE_DEFINE_GAMEPLAY_TAG(Walking, "VS.Movement.Mode.Walking");
			UE_DEFINE_GAMEPLAY_TAG(NavWalking, "VS.Movement.Mode.NavWalking");
			UE_DEFINE_GAMEPLAY_TAG(Falling, "VS.Movement.Mode.Falling");
			UE_DEFINE_GAMEPLAY_TAG(Swimming, "VS.Movement.Mode.Swimming");
			UE_DEFINE_GAMEPLAY_TAG(Flying, "VS.Movement.Mode.Flying");
		}
	}
}