// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSInputKeySelectorGroupWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/VSContentedInputKeySelector.h"
#include "Components/VSInputKeySelector.h"

UVSInputKeySelectorGroupWidget::UVSInputKeySelectorGroupWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bBlockKeySelectionChanged(false)
{
	bLocked = true;

	KeyBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
}

#if WITH_EDITOR
void UVSInputKeySelectorGroupWidget::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSInputKeySelectorGroupWidget, EditorPreviewKeys))
	{
		
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

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

TArray<FInputChord> UVSInputKeySelectorGroupWidget::GetKeys() const
{
	TArray<FInputChord> OutKeys;
	for (UVSContentedInputKeySelector* InputKeySelector : KeySelectorsPrivate)
	{
		bool bAddKey = false;
		if (InputKeySelector)
		{
			if (UVSInputKeySelector* Selector = InputKeySelector->GetInputKeySelector())
			{
				OutKeys.Add(Selector->GetSelectedKey());
				bAddKey = true;
			}
		}
		
		if (!bAddKey)
		{
			OutKeys.Add(FInputChord());
		}
	}
	return OutKeys;
}

void UVSInputKeySelectorGroupWidget::SetKeys(const TArray<FInputChord>& InKeys)
{
	const TArray<FInputChord> PrevKeys = GetKeys();
	
	bBlockKeySelectionChanged = true;
	for (int i = 0; i < InKeys.Num(); i++)
	{
		if (!KeySelectorsPrivate.IsValidIndex(i)) break;
		if (KeySelectorsPrivate[i])
		{
			if (UVSInputKeySelector* Selector = KeySelectorsPrivate[i]->GetInputKeySelector())
			{
				Selector->SetSelectedKey(InKeys[i]);
			}
		}
	}
	bBlockKeySelectionChanged = false;

	if (PrevKeys != GetKeys())
	{
		NotifyKeysUpdated();
	}
}

void UVSInputKeySelectorGroupWidget::SetKeyAtIndex(int32 Index, const FInputChord& InKey)
{
	if (!KeySelectorsPrivate.IsValidIndex(Index)) return;
	if (UVSInputKeySelector* Selector = KeySelectorsPrivate[Index]->GetInputKeySelector())
	{
		Selector->SetSelectedKey(InKey);
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
			if (UVSInputKeySelector* InputKeySelector = PrevKeySelectors[i]->GetInputKeySelector())
			{
				InputKeySelector->OnKeySelected.RemoveDynamic(this, &UVSInputKeySelectorGroupWidget::OnKeySelectionChanged);
			}
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
			KeySelectorsPrivate.Add(InputKeySelector);
			Panel_Keys->AddChild(InputKeySelector);
			KeyPanelSlotSettings.ApplyToWidget(InputKeySelector);

			InputKeySelector->KeyIconConfig = KeyIconConfig ? KeyIconConfig : nullptr;
			if (KeyBrush.GetDrawType() != ESlateBrushDrawType::NoDrawType)
			{
				InputKeySelector->KeyBrush = KeyBrush;
			}
			InputKeySelector->RefreshContents();

			UVSInputKeySelector* Selector = InputKeySelector->GetInputKeySelector();
			if (!Selector) continue;

			/** Allpy settings. */
			StyleSettings.ApplyToKeySelector(Selector);
			if (KeySettings.IsValidIndex(i))
			{
				KeySettings[i].ApplyToKeySelector(Selector);
			}
			
			Selector->OnKeySelected.AddDynamic(this, &UVSInputKeySelectorGroupWidget::OnKeySelectionChanged);
		}
	}
}

void UVSInputKeySelectorGroupWidget::NotifyKeysUpdated()
{
	OnKeysUpdated_Native.Broadcast(this);
	OnKeysUpdated.Broadcast(this);
}

void UVSInputKeySelectorGroupWidget::OnKeySelectionChanged(FInputChord SelectedKey)
{
	if (!bBlockKeySelectionChanged)
	{
		NotifyKeysUpdated();
	}
}
