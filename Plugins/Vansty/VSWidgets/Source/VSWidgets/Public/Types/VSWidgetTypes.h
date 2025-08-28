// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSMathTypes.h"
#include "UObject/Object.h"
#include "VSWidgetTypes.generated.h"

USTRUCT(BlueprintType)
struct FVSWidgetSubtitleParams : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubtitleText;

	/** Could be left empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpeakerNameTextStyleRowName = FName("Default");

	/**
	 * If assigned, the duration will be set to the voice's length.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> SourceVoice;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSAlphaDurationBlendArgs AlphaDurationBlendArgs;
};

USTRUCT(BlueprintType)
struct FVSWidgetNotifyParams : public FTableRowBase
{
	GENERATED_BODY()

	/** This should always be set. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NotifyTitleText;

	/** Optional. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NotifyContentText;
	
	/** Used as an icon. Optional. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush NotifyImage;
	
	/** Duration of this subtitle. Only works with invalid voice. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSAlphaDurationBlendArgs AlphaDurationBlendArgs;
};