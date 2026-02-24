// Copyright VanstySanic. All Rights Reserved.


#include "Components/VSInputKeySelector.h"
#include "VSPrivablic.h"
#include "Widgets/Input/SVSInputKeySelector.h"

VS_DECLARE_PRIVABLIC_MEMBER(UInputKeySelector, MyInputKeySelector, TSharedPtr<SInputKeySelector>);
VS_DECLARE_PRIVABLIC_METHOD_ADDRESS(UInputKeySelector, HandleKeySelected, void, const FInputChord&);

UVSInputKeySelector::UVSInputKeySelector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAllowKeyboardKeys(true)
	, bAllowMouseKeys(true)
{
	SetAllowGamepadKeys(true);
}

void UVSInputKeySelector::SetAllowKeyboardKeys(bool bInAllowKeyboardKeys)
{
	if (MyVSInputKeySelector.IsValid())
	{
		MyVSInputKeySelector->SetAllowKeyboardKeys(bInAllowKeyboardKeys);
	}
	bAllowKeyboardKeys = bInAllowKeyboardKeys;
}

bool UVSInputKeySelector::AllowKeyboardKeys() const
{
	return bAllowKeyboardKeys;
}

void UVSInputKeySelector::SetAllowMouseKeys(bool bInAllowMouseKeys)
{
	if (MyVSInputKeySelector.IsValid())
	{
		MyVSInputKeySelector->SetAllowMouseKeys(bInAllowMouseKeys);
	}
	bAllowMouseKeys = bInAllowMouseKeys;
}

bool UVSInputKeySelector::AllowMouseKeys() const
{
	return bAllowMouseKeys;
}

#if WITH_EDITOR
void UVSInputKeySelector::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputKeySelector, EditorPreviewKey))
	{
		SetSelectedKey(EditorPreviewKey);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UVSInputKeySelector::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyVSInputKeySelector.IsValid()) return;

	MyVSInputKeySelector->SetAllowKeyboardKeys(bAllowKeyboardKeys);
	MyVSInputKeySelector->SetAllowMouseKeys(bAllowMouseKeys);
}

TSharedRef<SWidget> UVSInputKeySelector::RebuildWidget()
{
	MyVSInputKeySelector = SNew(SVSInputKeySelector)
		.SuperArgs(
			SInputKeySelector::FArguments()
				.SelectedKey(GetSelectedKey())
				.Margin(GetMargin())
				.ButtonStyle(&GetButtonStyle())
				.TextStyle(&GetTextStyle())
				.KeySelectionText(GetKeySelectionText())
				.NoKeySpecifiedText(GetNoKeySpecifiedText())
				.AllowModifierKeys(AllowModifierKeys())
				.AllowGamepadKeys(AllowGamepadKeys())
				.EscapeKeys(EscapeKeys)
				.OnKeySelected(SInputKeySelector::FOnKeySelected::CreateUObject(this, VS_PRIVABLIC_METHOD_ADDRESS(UInputKeySelector, HandleKeySelected)))
				.OnIsSelectingKeyChanged(SInputKeySelector::FOnIsSelectingKeyChanged::CreateUObject(this, &UVSInputKeySelector::HandleIsSelectingKeyChanged))
		)
		.AllowKeyboardKeys(bAllowKeyboardKeys)
		.AllowMouseKeys(bAllowMouseKeys);

	VS_PRIVABLIC_MEMBER(this, UInputKeySelector, MyInputKeySelector) = MyVSInputKeySelector;

	return MyVSInputKeySelector.ToSharedRef();
}

void UVSInputKeySelector::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

#if WITH_EDITOR
	SetSelectedKey(EditorPreviewKey);
#endif
}

void UVSInputKeySelector::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyVSInputKeySelector.Reset();
}

void UVSInputKeySelector::HandleIsSelectingKeyChanged()
{
	OnIsSelectingKeyChanged.Broadcast();
}
