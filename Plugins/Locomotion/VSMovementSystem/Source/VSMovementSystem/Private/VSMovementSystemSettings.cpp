// Copyright VanstySanic. All Rights Reserved.

#include "VSMovementSystemSettings.h"
#include "Types/VSCharacterMovementTags.h"

#if WITH_EDITOR
FText UVSMovementSystemSettings::GetSectionText() const
{
	return FText::FromString("VSMovementSystem");
}
#endif

const UVSMovementSystemSettings* UVSMovementSystemSettings::Get()
{
	static const UVSMovementSystemSettings* DeveloperSettings = Cast<UVSMovementSystemSettings>(GetDefault<UVSMovementSystemSettings>());
	return DeveloperSettings;
}

FGameplayTag UVSMovementSystemSettings::GetTagFromMovementMode(const EMovementMode InMovementMode, const uint8 InCustomMode)
{
	static const TMap<EMovementMode, FGameplayTag> MovementModeTags =
	{
		{ MOVE_None, EVSMovementMode::None },
		{ MOVE_Walking, EVSMovementMode::Walking },
		{ MOVE_NavWalking, EVSMovementMode::NavWalking },
		{ MOVE_Falling, EVSMovementMode::Falling },
		{ MOVE_Swimming, EVSMovementMode::Swimming },
		{ MOVE_Flying, EVSMovementMode::Flying },
	};
	
	if (InMovementMode == MOVE_Custom)
	{
		return Get()->CustomMovementModeTags.FindRef(InCustomMode);
	}

	if (const FGameplayTag* MovementModeTagPtr = MovementModeTags.Find(InMovementMode))
	{
		return *MovementModeTagPtr;
	}

	return EVSMovementMode::None;
}

bool UVSMovementSystemSettings::GetMovementModeFromTag(const FGameplayTag& InTag, TEnumAsByte<EMovementMode>& OutMovementMode, uint8& OutCustomMode)
{
	static const TMap<EMovementMode, FGameplayTag> MovementModeTags =
	{
		{ MOVE_None, EVSMovementMode::None },
		{ MOVE_Walking, EVSMovementMode::Walking },
		{ MOVE_NavWalking, EVSMovementMode::NavWalking },
		{ MOVE_Falling, EVSMovementMode::Falling },
		{ MOVE_Swimming, EVSMovementMode::Swimming },
		{ MOVE_Flying, EVSMovementMode::Flying },
	};
	
	if (const EMovementMode* MovementModePtr = MovementModeTags.FindKey(InTag))
	{
		OutMovementMode = *MovementModePtr;
		OutCustomMode = 0;
		return true;
	}

	if (const uint8* CustomModePtr = Get()->CustomMovementModeTags.FindKey(InTag))
	{
		OutMovementMode = MOVE_Custom;
		OutCustomMode = *CustomModePtr;
		return true;
	}

	return false;
}