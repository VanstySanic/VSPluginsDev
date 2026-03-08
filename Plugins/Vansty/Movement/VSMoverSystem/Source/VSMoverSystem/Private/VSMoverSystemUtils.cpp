// Copyright VanstySanic. All Rights Reserved.

#include "VSMoverSystemUtils.h"
#include "VSMoverSystemSettings.h"
#include "Types/VSMovementSystemTags.h"

static const TMap<FName, FGameplayTag> DefaultMovementModeNames =
{
	{ "None", VS::Movement::Mode::None },
	{ "Walking", VS::Movement::Mode::Walking },
	{ "NavWalking", VS::Movement::Mode::NavWalking },
	{ "Falling", VS::Movement::Mode::Falling },
	{ "Swimming", VS::Movement::Mode::Swimming },
	{ "Flying", VS::Movement::Mode::Flying },
};


UVSMoverSystemUtils::UVSMoverSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FGameplayTag UVSMoverSystemUtils::GetUnnamedMovementMode(const FName ModeName)
{
	if (const FGameplayTag* MovementModeTagPtr = UVSMoverSystemSettings::Get()->CustomMovementModeTags.Find(ModeName))
	{
		return *MovementModeTagPtr;
	}
	
	if (const FGameplayTag* MovementModeTagPtr = DefaultMovementModeNames.Find(ModeName))
	{
		return *MovementModeTagPtr;
	}

	return VS::Movement::Mode::None;
}

FName UVSMoverSystemUtils::GetNamedMovementMode(const FGameplayTag& InTag)
{
	if (const FName* CustomModePtr = UVSMoverSystemSettings::Get()->CustomMovementModeTags.FindKey(InTag))
	{
		return *CustomModePtr;
	}
	
	if (const FName* MovementModePtr = DefaultMovementModeNames.FindKey(InTag))
	{
		return *MovementModePtr;
	}
	
	return NAME_None;
}
