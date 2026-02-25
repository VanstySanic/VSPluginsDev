// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSInputKeySelectorGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/VSContentedInputKeySelector.h"

UVSInputKeySelectorGroupWidget::UVSInputKeySelectorGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bLocked = true;

	KeyBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
}

void UVSInputKeySelectorGroupWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!bDifferRefreshment
#if WITH_EDITOR
		|| IsDesignTime()
#endif
	)
	{
		RefreshKeySelectors();

#if WITH_EDITOR
		if (IsDesignTime())
		{
			SetKeys(EditorPreviewKeys);
		}
#endif
	}
}

UVSContentedInputKeySelector* UVSInputKeySelectorGroupWidget::GetKeySelectorAtIndex(int32 Index) const
{
	return KeySelectorsPrivate.IsValidIndex(Index) ? KeySelectorsPrivate[Index] : nullptr;
}

FInputChord UVSInputKeySelectorGroupWidget::GetKeyAtIndex(int32 Index) const
{
	return CurrentKeys.IsValidIndex(Index) ? CurrentKeys[Index] : FInputChord();
}

void UVSInputKeySelectorGroupWidget::SetKeys(const TArray<FInputChord>& InKeys)
{
	const TArray<FInputChord> PrevKeys = CurrentKeys;
	CurrentKeys = InKeys;

	/** Update selector key in order. */
	for (int i = 0; i < CurrentKeys.Num(); i++)
	{
		if (!KeySelectorsPrivate.IsValidIndex(i)) break;
		if (KeySelectorsPrivate[i])
		{
			KeySelectorsPrivate[i]->OnKeySelected_Native.RemoveAll(this);
			KeySelectorsPrivate[i]->SetSelectedKey(CurrentKeys[i]);
			KeySelectorsPrivate[i]->OnKeySelected_Native.AddUObject(this, &UVSInputKeySelectorGroupWidget::OnSelectorKeySelected);
		}
	}

	/** Clear unset keys in selectors. */
	for (int i = CurrentKeys.Num(); i < KeySelectorsPrivate.Num(); i++)
	{
		if (KeySelectorsPrivate[i])
		{
			KeySelectorsPrivate[i]->OnKeySelected_Native.RemoveAll(this);
			KeySelectorsPrivate[i]->SetSelectedKey(FInputChord());
			KeySelectorsPrivate[i]->OnKeySelected_Native.AddUObject(this, &UVSInputKeySelectorGroupWidget::OnSelectorKeySelected);
		}
	}

	if (PrevKeys != CurrentKeys)
	{
		NotifyKeysUpdated();
	}
}

void UVSInputKeySelectorGroupWidget::SetKeyAtIndex(int32 Index, const FInputChord& InKey)
{
	const FInputChord PrevKey = GetKeyAtIndex(Index);
	
	if (CurrentKeys.Num() <= Index)
	{
		CurrentKeys.SetNum(Index + 1);
	}
	if (CurrentKeys.IsValidIndex(Index))
	{
		CurrentKeys[Index] = InKey;
	}
	
	if (KeySelectorsPrivate.IsValidIndex(Index) && KeySelectorsPrivate[Index])
	{
		KeySelectorsPrivate[Index]->OnKeySelected_Native.RemoveAll(this);
		KeySelectorsPrivate[Index]->SetSelectedKey(InKey);
		KeySelectorsPrivate[Index]->OnKeySelected_Native.AddUObject(this, &UVSInputKeySelectorGroupWidget::OnSelectorKeySelected);
	}

	if (PrevKey != GetKeyAtIndex(Index))
	{
		NotifyKeySelected(Index, InKey);
		NotifyKeysUpdated();
	}
}

void UVSInputKeySelectorGroupWidget::RefreshKeySelectors()
{
	if (!Panel_Keys || !InputKeySelectorClass) return;
	
	/** Remove current. */
	const TArray<UVSContentedInputKeySelector*> PrevKeySelectors = KeySelectorsPrivate;
	for (int i = 0; i < PrevKeySelectors.Num(); i++)
	{
		if (PrevKeySelectors[i])
		{
			PrevKeySelectors[i]->OnKeySelected_Native.RemoveAll(this);
			PrevKeySelectors[i]->RemoveFromParent();
		}
	}
	
	KeySelectorsPrivate.Empty();
	Panel_Keys->ClearChildren();

	/** Generate new. */
	for (int i = 0; i < KeyNum; i++)
	{
		if (UVSContentedInputKeySelector* InputKeySelector = WidgetTree->ConstructWidget<UVSContentedInputKeySelector>(InputKeySelectorClass))
		{
			if (CurrentKeys.IsValidIndex(i))
			{
				InputKeySelector->SetSelectedKey(CurrentKeys[i]);
			}
			
			KeySelectorsPrivate.Add(InputKeySelector);
			Panel_Keys->AddChild(InputKeySelector);
			KeySlotSettings.ApplyToWidget(InputKeySelector);
			
			InputKeySelector->KeyIconConfig = KeyIconConfig ? KeyIconConfig : nullptr;
			if (KeyBrush.GetDrawType() != ESlateBrushDrawType::NoDrawType)
			{
				InputKeySelector->KeyBrush = KeyBrush;
			}
			
			InputKeySelector->RefreshContents();

			InputKeySelector->OnKeySelected_Native.AddUObject(this, &UVSInputKeySelectorGroupWidget::OnSelectorKeySelected);

			/** Allpy settings. */
			StyleSettings.ApplyToKeySelector(InputKeySelector);
			KeySettings.ApplyToKeySelector(InputKeySelector);
		}
	}
}

void UVSInputKeySelectorGroupWidget::NotifyKeySelected(int32 Index, FInputChord SelectedKey)
{
	OnKeySelected_Native.Broadcast(this, Index, SelectedKey);
	OnKeySelected.Broadcast(this, Index, SelectedKey);
}

void UVSInputKeySelectorGroupWidget::NotifyKeysUpdated()
{
	OnKeysUpdated_Native.Broadcast(this);
	OnKeysUpdated.Broadcast(this);
}

void UVSInputKeySelectorGroupWidget::OnSelectorKeySelected(UVSContentedInputKeySelector* Selector, FInputChord SelectedKey)
{
	for (int i = 0; i < KeySelectorsPrivate.Num(); ++i)
	{
		if (Selector && KeySelectorsPrivate[i] == Selector)
		{
			const FInputChord PrevKey = GetKeyAtIndex(i);

			if (CurrentKeys.Num() <= i)
			{
				CurrentKeys.SetNum(i + 1);
			}
			CurrentKeys[i] = SelectedKey;

			if (PrevKey != SelectedKey)
			{
				NotifyKeySelected(i, SelectedKey);
				NotifyKeysUpdated();
			}
			
			break;
		}
	}
}
