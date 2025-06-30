// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSCharacterMovementTags.h"

namespace EVSMovementMode
{
	UE_DEFINE_GAMEPLAY_TAG(None, "VSLocomotionSystem.MovementMode.None");
	UE_DEFINE_GAMEPLAY_TAG(Walking, "VSLocomotionSystem.MovementMode.Walking");
	UE_DEFINE_GAMEPLAY_TAG(NavWalking, "VSLocomotionSystem.MovementMode.NavWalking");
	UE_DEFINE_GAMEPLAY_TAG(Falling, "VSLocomotionSystem.MovementMode.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Swimming, "VSLocomotionSystem.MovementMode.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Flying, "VSLocomotionSystem.MovementMode.Flying");
	
	UE_DEFINE_GAMEPLAY_TAG(Mantling, "VSLocomotionSystem.MovementMode.Mantling");
	UE_DEFINE_GAMEPLAY_TAG(Vaulting, "VSLocomotionSystem.MovementMode.Vaulting");
	UE_DEFINE_GAMEPLAY_TAG(WallRunning, "VSLocomotionSystem.MovementMode.WallRunning");
	UE_DEFINE_GAMEPLAY_TAG(FixedPointLeap, "VSLocomotionSystem.MovementMode.FixedPointLeap");
}

namespace EVSOrientationEvaluateType
{
	UE_DEFINE_GAMEPLAY_TAG(None, "VSLocomotionSystem.Orientation.EvaluateType.None");
	UE_DEFINE_GAMEPLAY_TAG(Self, "VSLocomotionSystem.Orientation.EvaluateType.Self");
	UE_DEFINE_GAMEPLAY_TAG(Velocity, "VSLocomotionSystem.Orientation.EvaluateType.Velocity");
	UE_DEFINE_GAMEPLAY_TAG(Input, "VSLocomotionSystem.Orientation.EvaluateType.Input");
	UE_DEFINE_GAMEPLAY_TAG(Movement, "VSLocomotionSystem.Orientation.EvaluateType.Movement")
	UE_DEFINE_GAMEPLAY_TAG(Control, "VSLocomotionSystem.Orientation.EvaluateType.Control");
	UE_DEFINE_GAMEPLAY_TAG(View, "VSLocomotionSystem.Orientation.EvaluateType.View");
	UE_DEFINE_GAMEPLAY_TAG(Aiming, "VSLocomotionSystem.Orientation.EvaluateType.Aiming");
}

namespace EVSStance
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, "VSLocomotionSystem.Stance.Standing");
	UE_DEFINE_GAMEPLAY_TAG(Crouching, "VSLocomotionSystem.Stance.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Crawling, "VSLocomotionSystem.Stance.Crawling");
}

namespace EVSGait
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, "VSLocomotionSystem.Gait.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Jogging, "VSLocomotionSystem.Gait.Jogging");
	UE_DEFINE_GAMEPLAY_TAG(Running, "VSLocomotionSystem.Gait.Running");
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, "VSLocomotionSystem.Gait.Sprinting");
}

namespace EVSMovementState
{
	UE_DEFINE_GAMEPLAY_TAG(TurnInPlace, "VSLocomotionSystem.MovementState.TurnInPlace");
	UE_DEFINE_GAMEPLAY_TAG(LandRecovery, "VSLocomotionSystem.MovementState.LandRecovery");
}

namespace EVSMovementNotify
{
	UE_DEFINE_GAMEPLAY_TAG(StateChange_MovementMode, "VSLocomotionSystem.MovementNotify.StateChange.MovementMode");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Stance, "VSLocomotionSystem.MovementNotify.StateChange.Stance");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Gait, "VSLocomotionSystem.MovementNotify.StateChange.Gait");
	
	UE_DEFINE_GAMEPLAY_TAG(Action_Jump, "VSLocomotionSystem.MovementNotify.Action.Jump");
	
	// UE_DEFINE_GAMEPLAY_TAG(Action_TurnInPlace, "VSLocomotionSystem.MovementNotify.Action.TurnInPlace");
	// UE_DEFINE_GAMEPLAY_TAG(Action_LandRecovery, "VSLocomotionSystem.MovementNotify.Action.LandRecovery");
}