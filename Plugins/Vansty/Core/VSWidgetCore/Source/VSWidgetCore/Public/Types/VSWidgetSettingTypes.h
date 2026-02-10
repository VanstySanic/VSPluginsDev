// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSWidgetSettingTypes.generated.h"

class UCommonBoundActionButton;
class UCommonActionWidget;
class UCommonButtonBase;
class UCommonButtonStyle;

USTRUCT(BlueprintType)
struct VSWIDGETCORE_API FVSCommonActionWidgetSettings
{
	GENERATED_BODY()
	
	FVSCommonActionWidgetSettings(const UCommonActionWidget* ActionWidget = nullptr);
	void ApplyToAction(UCommonActionWidget* ActionWidget) const;
	
	/**
	 * List all the input actions that this common action widget is intended to represent.  In some cases you might have multiple actions
	 * that you need to represent as a single entry in the UI.  For example - zoom, might be mouse wheel up or down, but you just need to
	 * show a single icon for Up & Down on the mouse, this solves that problem.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideInputActions", RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	TArray<FDataTableRowHandle> InputActions;

	/** Input Action this common action widget is intended to represent. Optional if using EnhancedInputs. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideEnhancedInputAction"))
	TObjectPtr<class UInputAction> EnhancedInputAction;

	/**
	 * The material to use when showing held progress, the progress will be sent using the material parameter
	 * defined by ProgressMaterialParam and the value will range from 0..1.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideProgressMaterialBrush"))
	FSlateBrush ProgressMaterialBrush;

	/** The material parameter on ProgressMaterialBrush to update the held percentage.  This value will be 0..1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideProgressMaterialParam"))
	FName ProgressMaterialParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideIconRimBrush"))
	FSlateBrush IconRimBrush;


	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideInputActions : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideEnhancedInputAction : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideProgressMaterialBrush : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideProgressMaterialParam : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideIconRimBrush : 1;
};

USTRUCT(BlueprintType)
struct VSWIDGETCORE_API FVSCommonButtonStyleSettings
{
	GENERATED_BODY()

	FVSCommonButtonStyleSettings(const UCommonButtonBase* Button = nullptr);
	void ApplyToButton(UCommonButtonBase* Button) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideStyle"))
	TSubclassOf<UCommonButtonStyle> Style;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideMinWidth"))
	int32 MinWidth = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideMinHeight"))
	int32 MinHeight = 0;
	
	
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideStyle : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideMinWidth : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideMinHeight : 1;
};

USTRUCT(BlueprintType)
struct VSWIDGETCORE_API FVSCommonButtonActionSettings
{
	GENERATED_BODY()

	FVSCommonButtonActionSettings(const UCommonButtonBase* Button = nullptr);
	void ApplyToButton(UCommonButtonBase* Button) const;

	/**
	 *	The input action that is bound to this button.
	 *	The common input manager will trigger this button to click if the action was pressed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideInputAction", RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle InputAction;

	/**
	 * The enhanced input action that is bound to this button.
	 * The common input manager will trigger this button to click if the action was pressed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideEnhancedInputAction"))
	TObjectPtr<UInputAction> EnhancedInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverridePriority"))
	int32 Priority = 0;

	/** this only works when the button has no input action widget. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideActionName"))
	FText ActionName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSCommonActionWidgetSettings ActionDisplaySettings;

	/** this only works when the button has no input action widget. */
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideInputAction : 1;

	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideEnhancedInputAction : 1;
	
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverridePriority : 1;
	
	/** Whether to override the action's display name over the action handle's original one. */
	UPROPERTY(BlueprintReadWrite)
	uint8 bOverrideActionName : 1;
};