// Copyright VanstySanic. All Rights Reserved.

#include "VSCharacterMovementSystemUtils.h"
#include "VSCharacterMovementSystemSettings.h"
#include "Types/VSMovementSystemTags.h"

static const TMap<EMovementMode, FGameplayTag> DefaultMovementModeTags =
{
	{ MOVE_None, VS::Movement::Mode::None },
	{ MOVE_Walking, VS::Movement::Mode::Walking },
	{ MOVE_NavWalking, VS::Movement::Mode::NavWalking },
	{ MOVE_Falling, VS::Movement::Mode::Falling },
	{ MOVE_Swimming, VS::Movement::Mode::Swimming },
	{ MOVE_Flying, VS::Movement::Mode::Flying },
};


UVSCharacterMovementSystemUtils::UVSCharacterMovementSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FGameplayTag UVSCharacterMovementSystemUtils::GetTaggedMovementMode(const EMovementMode InMovementMode, const uint8 InCustomMode)
{
	if (InMovementMode == MOVE_Custom && UVSCharacterMovementSystemSettings::Get()->CustomMovementModeTags.Contains(InCustomMode))
	{
		return UVSCharacterMovementSystemSettings::Get()->CustomMovementModeTags[InCustomMode];
	}

	if (const FGameplayTag* MovementModeTagPtr = DefaultMovementModeTags.Find(InMovementMode))
	{
		return *MovementModeTagPtr;
	}

	return VS::Movement::Mode::None;
}

bool UVSCharacterMovementSystemUtils::GetUntaggedMovementMode(const FGameplayTag& InTag,TEnumAsByte<EMovementMode>& OutMovementMode, uint8& OutCustomMode)
{
	if (const uint8* CustomModePtr = UVSCharacterMovementSystemSettings::Get()->CustomMovementModeTags.FindKey(InTag))
	{
		OutMovementMode = MOVE_Custom;
		OutCustomMode = *CustomModePtr;
		return true;
	}
	
	if (const EMovementMode* MovementModePtr = DefaultMovementModeTags.FindKey(InTag))
	{
		OutMovementMode = *MovementModePtr;
		OutCustomMode = 0;
		return true;
	}
	
	return false;
}
