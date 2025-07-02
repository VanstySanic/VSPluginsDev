// Copyright VanstySanic. All Rights Reserved.

#include "Types/VSCharacterMovementTags.h"

namespace EVSMovementMode
{
	UE_DEFINE_GAMEPLAY_TAG(None, "VSMovementSystem.State.MovementMode.None");
	UE_DEFINE_GAMEPLAY_TAG(Walking, "VSMovementSystem.State.MovementMode.Walking");
	UE_DEFINE_GAMEPLAY_TAG(NavWalking, "VSMovementSystem.State.MovementMode.NavWalking");
	UE_DEFINE_GAMEPLAY_TAG(Falling, "VSMovementSystem.State.MovementMode.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Swimming, "VSMovementSystem.State.MovementMode.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Flying, "VSMovementSystem.State.MovementMode.Flying");
	
	UE_DEFINE_GAMEPLAY_TAG(Mantling, "VSMovementSystem.MovementMode.Mantling");
	UE_DEFINE_GAMEPLAY_TAG(Vaulting, "VSMovementSystem.MovementMode.Vaulting");
	UE_DEFINE_GAMEPLAY_TAG(WallRunning, "VSMovementSystem.MovementMode.WallRunning");
	UE_DEFINE_GAMEPLAY_TAG(FixedPointLeap, "VSMovementSystem.MovementMode.FixedPointLeap");
}

namespace EVSOrientationEvaluateType
{
	UE_DEFINE_GAMEPLAY_TAG(None, "VSMovementSystem.Orientation.EvaluateType.None");
	UE_DEFINE_GAMEPLAY_TAG(Self, "VSMovementSystem.Orientation.EvaluateType.Self");
	UE_DEFINE_GAMEPLAY_TAG(Velocity, "VSMovementSystem.Orientation.EvaluateType.Velocity");
	UE_DEFINE_GAMEPLAY_TAG(Input, "VSMovementSystem.Orientation.EvaluateType.Input");
	UE_DEFINE_GAMEPLAY_TAG(Movement, "VSMovementSystem.Orientation.EvaluateType.Movement")
	UE_DEFINE_GAMEPLAY_TAG(Control, "VSMovementSystem.Orientation.EvaluateType.Control");
	UE_DEFINE_GAMEPLAY_TAG(View, "VSMovementSystem.Orientation.EvaluateType.View");
	UE_DEFINE_GAMEPLAY_TAG(Aiming, "VSMovementSystem.Orientation.EvaluateType.Aiming");
}

namespace EVSStance
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, "VSMovementSystem.State.Stance.Standing");
	UE_DEFINE_GAMEPLAY_TAG(Crouching, "VSMovementSystem.State.Stance.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Crawling, "VSMovementSystem.State.Stance.Crawling");
}

namespace EVSGait
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, "VSMovementSystem.State.Gait.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Jogging, "VSMovementSystem.State.Gait.Jogging");
	UE_DEFINE_GAMEPLAY_TAG(Running, "VSMovementSystem.State.Gait.Running");
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, "VSMovementSystem.State.Gait.Sprinting");
}

namespace EVSMovementState
{
	UE_DEFINE_GAMEPLAY_TAG(TurnInPlace, "VSMovementSystem.State.TurnInPlace");
	UE_DEFINE_GAMEPLAY_TAG(LandRecovery, "VSMovementSystem.State.LandRecovery");
}

namespace EVSMovementEvent
{
	UE_DEFINE_GAMEPLAY_TAG(StateChange_MovementMode, "VSMovementSystem.Event.StateChange.MovementMode");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Stance, "VSMovementSystem.Event.StateChange.Stance");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Gait, "VSMovementSystem.Event.StateChange.Gait");
	
	UE_DEFINE_GAMEPLAY_TAG(Action_Jump, "VSMovementSystem.Event.Action.Jump");
	
	// UE_DEFINE_GAMEPLAY_TAG(Action_TurnInPlace, "VSMovementSystem.MovementNotify.Action.TurnInPlace");
	// UE_DEFINE_GAMEPLAY_TAG(Action_LandRecovery, "VSMovementSystem.MovementNotify.Action.LandRecovery");
}