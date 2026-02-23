// Copyright VanstySanic. All Rights Reserved.

#include "Widgets/Input/SVSInputKeySelector.h"
#include "SlateOptMacros.h"
#include "VSPrivablic.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

VS_DECLARE_PRIVABLIC_MEMBER(SInputKeySelector, bAllowGamepadKeys, bool);
VS_DECLARE_PRIVABLIC_MEMBER(SInputKeySelector, bEscapeCancelsSelection, bool);

VS_DECLARE_PRIVABLIC_METHOD(SInputKeySelector, SetIsSelectingKey, void, bool);
VS_DECLARE_PRIVABLIC_METHOD_CONST(SInputKeySelector, IsEscapeKey, bool, const FKey&);
VS_DECLARE_PRIVABLIC_METHOD(SInputKeySelector, SelectKey, void, FKey, bool, bool, bool, bool);

void SVSInputKeySelector::Construct(const FArguments& InArgs)
{
	SInputKeySelector::Construct(InArgs._SuperArgs);
	
	bAllowKeyboardKeys = InArgs._AllowKeyboardKeys;
	bAllowMouseKeys = InArgs._AllowMouseKeys;
}

void SVSInputKeySelector::SetAllowKeyboardKeys(bool bInAllowKeyboardKeys)
{
	bAllowKeyboardKeys = bInAllowKeyboardKeys;
}

void SVSInputKeySelector::SetAllowMouseKeys(bool bInAllowMouseKeys)
{
	bAllowMouseKeys = bInAllowMouseKeys;
}

FReply SVSInputKeySelector::OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (GetIsSelectingKey() && !bAllowKeyboardKeys && !Key.IsGamepadKey() && Key != EKeys::Escape)
	{
		return FReply::Handled();
	}

	return SInputKeySelector::OnPreviewKeyDown(MyGeometry, InKeyEvent);
}

FReply SVSInputKeySelector::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const FKey Key = MouseEvent.GetEffectingButton();
	if (GetIsSelectingKey() && !bAllowMouseKeys && Key.IsMouseButton())
	{
		return FReply::Handled();
	}

	return SInputKeySelector::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SVSInputKeySelector::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FKey KeyUp = InKeyEvent.GetKey();
	
	if (KeyUp == EKeys::Escape || VS_PRIVABLIC_METHOD_CONST(this, SInputKeySelector, IsEscapeKey)(KeyUp))
	{
		VS_PRIVABLIC_METHOD(this, SInputKeySelector, SetIsSelectingKey)(false);
		return FReply::Handled();
	}
	
	EModifierKey::Type ModifierKey = EModifierKey::FromBools(
		InKeyEvent.IsControlDown() && KeyUp != EKeys::LeftControl && KeyUp != EKeys::RightControl,
		InKeyEvent.IsAltDown() && KeyUp != EKeys::LeftAlt && KeyUp != EKeys::RightAlt,
		InKeyEvent.IsShiftDown() && KeyUp != EKeys::LeftShift && KeyUp != EKeys::RightShift,
		InKeyEvent.IsCommandDown() && KeyUp != EKeys::LeftCommand && KeyUp != EKeys::RightCommand );
	
	bool bAllowSetKey = true;
	if (!GetIsSelectingKey() || !(KeyUp.IsModifierKey() == false || ModifierKey == EModifierKey::None)) bAllowSetKey = false;
	if (KeyUp.IsGamepadKey() && !VS_PRIVABLIC_MEMBER(this, SInputKeySelector, bAllowGamepadKeys)) bAllowSetKey = false;
	if (KeyUp.IsMouseButton() && !bAllowMouseKeys) bAllowSetKey = false;
	if (!KeyUp.IsGamepadKey() && !KeyUp.IsMouseButton() && !bAllowKeyboardKeys) bAllowSetKey = false;

	if (bAllowSetKey)
	{
		VS_PRIVABLIC_METHOD(this, SInputKeySelector, SetIsSelectingKey)(false);
		
		VS_PRIVABLIC_METHOD(this, SInputKeySelector, SelectKey)(
			KeyUp,
			ModifierKey == EModifierKey::Shift,
			ModifierKey == EModifierKey::Control,
			ModifierKey == EModifierKey::Alt,
			ModifierKey == EModifierKey::Command);
		
		return FReply::Handled();
	}
	
	if (!GetIsSelectingKey())
	{
		return SInputKeySelector::OnKeyUp(MyGeometry, InKeyEvent);
	}

	return SCompoundWidget::OnKeyUp(MyGeometry, InKeyEvent);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
