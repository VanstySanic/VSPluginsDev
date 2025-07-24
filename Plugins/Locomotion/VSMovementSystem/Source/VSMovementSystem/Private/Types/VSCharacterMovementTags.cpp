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
	
	UE_DEFINE_GAMEPLAY_TAG(MantlingOrVaulting, "VSMovementSystem.State.MovementMode.MantlingOrVaulting");
	UE_DEFINE_GAMEPLAY_TAG(WallRunning, "VSMovementSystem.State.MovementMode.WallRunning");
	UE_DEFINE_GAMEPLAY_TAG(FixedPointLeap, "VSMovementSystem.State.MovementMode.FixedPointLeap");
}

namespace EVSOrientationEvaluateType
{
	UE_DEFINE_GAMEPLAY_TAG(None, "VSMovementSystem.Orientation.EvaluateType.None");
	UE_DEFINE_GAMEPLAY_TAG(Self, "VSMovementSystem.Orientation.EvaluateType.Self");
	UE_DEFINE_GAMEPLAY_TAG(Velocity, "VSMovementSystem.Orientation.EvaluateType.Velocity");
	UE_DEFINE_GAMEPLAY_TAG(Input, "VSMovementSystem.Orientation.EvaluateType.Input");
	UE_DEFINE_GAMEPLAY_TAG(Movement, "VSMovementSystem.Orientation.EvaluateType.Movement")
	UE_DEFINE_GAMEPLAY_TAG(Control, "VSMovementSystem.Orientation.EvaluateType.Control");
	UE_DEFINE_GAMEPLAY_TAG(Aim_Component, "VSMovementSystem.Orientation.EvaluateType.Aim.Component");
	UE_DEFINE_GAMEPLAY_TAG(Aim_Point, "VSMovementSystem.Orientation.EvaluateType.Aim.Point");
	UE_DEFINE_GAMEPLAY_TAG(Aim_Direction, "VSMovementSystem.Orientation.EvaluateType.Aim.Direction");
}

namespace EVSStance
{
	UE_DEFINE_GAMEPLAY_TAG(Standing, "VSMovementSystem.State.Stance.Standing");
	UE_DEFINE_GAMEPLAY_TAG(Crouching, "VSMovementSystem.State.Stance.Crouching");

	/** Equals to Crouching in most cases. This is used to avoid the character movement original crouching state. */
	UE_DEFINE_GAMEPLAY_TAG(Squatting, "VSMovementSystem.State.Stance.Squatting");
	UE_DEFINE_GAMEPLAY_TAG(Crawling, "VSMovementSystem.State.Stance.Crawling");
}

namespace EVSGait
{
	UE_DEFINE_GAMEPLAY_TAG(Walking, "VSMovementSystem.State.Gait.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Running, "VSMovementSystem.State.Gait.Running");
	UE_DEFINE_GAMEPLAY_TAG(Sprinting, "VSMovementSystem.State.Gait.Sprinting");
}

namespace EVSMovementState
{
	UE_DEFINE_GAMEPLAY_TAG(IsMoving2D, "VSMovementSystem.State.IsMoving2D");
	UE_DEFINE_GAMEPLAY_TAG(HasMovementInput2D, "VSMovementSystem.State.HasMovementInput2D");


	UE_DEFINE_GAMEPLAY_TAG(TurnInPlace, "VSMovementSystem.State.TurnInPlace");
	UE_DEFINE_GAMEPLAY_TAG(LandRecovery, "VSMovementSystem.State.LandRecovery");
}

namespace EVSWallRunState
{
	UE_DEFINE_GAMEPLAY_TAG(Starting, "VSMovementSystem.State.WallRun.Starting");
	UE_DEFINE_GAMEPLAY_TAG(Cycling, "VSMovementSystem.State.WallRun.Cycling");
	UE_DEFINE_GAMEPLAY_TAG(Ending, "VSMovementSystem.State.WallRun.Ending");
}

namespace EVSMovementEvent
{
	UE_DEFINE_GAMEPLAY_TAG(StateChange_MovementMode, "VSMovementSystem.Event.StateChange.MovementMode");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Stance, "VSMovementSystem.Event.StateChange.Stance");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_Gait, "VSMovementSystem.Event.StateChange.Gait");
	
	UE_DEFINE_GAMEPLAY_TAG(StateChange_IsMoving2D, "VSMovementSystem.Event.StateChange.IsMoving2D");
	UE_DEFINE_GAMEPLAY_TAG(StateChange_HasMovementInput2D, "VSMovementSystem.Event.StateChange.HasMovementInput2D");
	
	UE_DEFINE_GAMEPLAY_TAG(Action_Jump, "VSMovementSystem.Event.Action.Jump");
	
	UE_DEFINE_GAMEPLAY_TAG(Action_TurnInPlace_Start, "VSMovementSystem.Event.Action.TurnInPlace.Start");
	UE_DEFINE_GAMEPLAY_TAG(Action_TurnInPlace_Connect, "VSMovementSystem.Event.Action.Connect");
	UE_DEFINE_GAMEPLAY_TAG(Action_TurnInPlace_End, "VSMovementSystem.Event.Action.End");
}