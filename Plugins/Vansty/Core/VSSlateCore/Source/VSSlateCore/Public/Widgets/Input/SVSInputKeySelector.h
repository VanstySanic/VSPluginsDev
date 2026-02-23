// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SInputKeySelector.h"

/**
 * 
 */
class VSSLATECORE_API SVSInputKeySelector : public SInputKeySelector
{
public:
	SLATE_BEGIN_ARGS(SVSInputKeySelector)
		: _AllowKeyboardKeys(true)
		, _AllowMouseKeys(true)
		{
		}

		SLATE_ARGUMENT(SInputKeySelector::FArguments, SuperArgs)
		SLATE_ARGUMENT(bool, AllowKeyboardKeys)
		SLATE_ARGUMENT(bool, AllowMouseKeys)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetAllowKeyboardKeys(bool bInAllowKeyboardKeys);
	void SetAllowMouseKeys(bool bInAllowMouseKeys);

public:
	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	bool bAllowKeyboardKeys = true;
	bool bAllowMouseKeys = true;
};
