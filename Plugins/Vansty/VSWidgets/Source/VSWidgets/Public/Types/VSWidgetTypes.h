// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SlateWrapperTypes.h"
#include "Types/VSMathTypes.h"
#include "UObject/Object.h"
#include "VSWidgetTypes.generated.h"

class UWidget;
class UCommonButtonBase;
class UCommonButtonStyle;
class UVSMessageDialog;

USTRUCT(BlueprintType)
struct FVSWidgetLayoutParams
{
	GENERATED_BODY()

	void ApplyToWidget(UWidget* Widget);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateChildSize Size = FSlateChildSize(ESlateSizeRule::Fill);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMargin Padding = FMargin();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment = HAlign_Fill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVerticalAlignment> VerticalAlignment = VAlign_Fill;
};

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

USTRUCT(BlueprintType)
struct FVSWidgetMessageDialogParams : public FTableRowBase
{
	GENERATED_BODY()

	FVSWidgetMessageDialogParams()
	{
		DialogTitleText = FText::FromString("Title");
		MessageContentText = FText::FromString("Message");
		DialogReplies = TArray<FName>
			{
				FName("Yes"),
				FName("No")
			};
	}

	/** Optional. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DialogTitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText MessageContentText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> DialogReplies;

	/** If not assigned, no button will be automatically focused. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultFocusReply = FName("Yes"); 
};

USTRUCT(BlueprintType)
struct FVSCommonButtonDisplayParams : public FTableRowBase
{
	GENERATED_BODY()

	VSWIDGETS_API void ApplyToButton(UCommonButtonBase* Button) const;

	/** Override if not none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::FromString("None");

	/** Override if not none. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCommonButtonStyle> Style;

	/**
	 * List all the input actions that this common action widget is intended to represent.  In some cases you might have multiple actions
	 * that you need to represent as a single entry in the UI.  For example - zoom, might be mouse wheel up or down, but you just need to
	 * show a single icon for Up & Down on the mouse, this solves that problem.
	 * Override if not none.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	TArray<FDataTableRowHandle> InputActions;

	/**
	 * Input Action this common action widget is intended to represent. Optional if using EnhancedInputs.
	 * Override if not none.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UInputAction> EnhancedInputAction;
};
