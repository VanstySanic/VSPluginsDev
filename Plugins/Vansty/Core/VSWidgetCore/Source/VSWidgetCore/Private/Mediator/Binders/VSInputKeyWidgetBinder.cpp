// Copyright VanstySanic. All Rights Reserved.
 
#include "Mediator/Binders/VSInputKeyWidgetBinder.h"
#include "Components/InputKeySelector.h"
#include "Components/VSContentedInputKeySelector.h"
#include "Components/VSInputKeySelectorGroupWidget.h"

UVSInputKeyWidgetBinder::UVSInputKeyWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInputKeyWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshKeys();
}

void UVSInputKeyWidgetBinder::Uninitialize_Implementation()
{
	Super::Uninitialize_Implementation();
}

void UVSInputKeyWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Keys"))
	{
		const TArray<FInputChord> ExternalKeys = GetExternalKeys();
		const FInputChord FirstExternalKey = ExternalKeys.IsValidIndex(0) ? ExternalKeys[0] : FInputChord();
			
		if (UVSInputKeySelectorGroupWidget* KeySelectorGroup = Cast<UVSInputKeySelectorGroupWidget>(Widget))
		{
			KeySelectorGroup->KeySettings = KeySettings;
			KeySelectorGroup->RefreshKeySelectors();
			KeySelectorGroup->SetKeys(ExternalKeys);

			KeySelectorGroup->OnKeysUpdated_Native.AddUObject(this, &UVSInputKeyWidgetBinder::OnInputKeyGroupUpdated);
		}
		else if (UVSContentedInputKeySelector* ContentedInputKeySelector = Cast<UVSContentedInputKeySelector>(Widget))
		{
			KeySettings.ApplyToKeySelector(ContentedInputKeySelector);
			ContentedInputKeySelector->SetSelectedKey(FirstExternalKey);
			ContentedInputKeySelector->OnKeySelected_Native.AddUObject(this, &UVSInputKeyWidgetBinder::OnContentedSelectorKeySelected);
		}
		else if (UInputKeySelector* InputKeySelector = Cast<UInputKeySelector>(Widget))
		{
			KeySettings.ApplyToKeySelector(InputKeySelector);
			InputKeySelector->SetSelectedKey(FirstExternalKey);
			
			InputKeySelector->OnKeySelected.AddDynamic(this, &UVSInputKeyWidgetBinder::OnInputKeySelectorKeySelected);
		}
	}
}

void UVSInputKeyWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Keys"))
	{
		if (UVSInputKeySelectorGroupWidget* KeySelectorGroup = Cast<UVSInputKeySelectorGroupWidget>(Widget))
		{
			KeySelectorGroup->OnKeysUpdated_Native.RemoveAll(this);
		}
		else if (UVSContentedInputKeySelector* ContentedInputKeySelector = Cast<UVSContentedInputKeySelector>(Widget))
		{
			ContentedInputKeySelector->OnKeySelected_Native.RemoveAll(this);
		}
		else if (UInputKeySelector* InputKeySelector = Cast<UInputKeySelector>(Widget))
		{
			InputKeySelector->OnKeySelected.RemoveDynamic(this, &UVSInputKeyWidgetBinder::OnInputKeySelectorKeySelected);
		}
	}

	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}


TArray<FInputChord> UVSInputKeyWidgetBinder::GetExternalKeys_Implementation() const
{
	return TArray<FInputChord>();
}

TArray<FInputChord> UVSInputKeyWidgetBinder::GetWidgetKeys_Implementation() const
{
	if (UWidget* KeysWidget = GetBoundTypedWidget(FName("Keys")))
	{
		if (const UVSInputKeySelectorGroupWidget* KeySelectorGroup = Cast<UVSInputKeySelectorGroupWidget>(KeysWidget))
		{
			return KeySelectorGroup->GetKeys();
		}
		if (const UVSContentedInputKeySelector* ContentedInputKeySelector = Cast<UVSContentedInputKeySelector>(KeysWidget))
		{
			return { ContentedInputKeySelector->GetSelectedKey() };
		}
		if (const UInputKeySelector* InputKeySelector = Cast<UInputKeySelector>(KeysWidget))
		{
			return { InputKeySelector->GetSelectedKey() };
		}
	}
	
	return TArray<FInputChord>();
}

FInputChord UVSInputKeyWidgetBinder::GetExternalKeyAtIndex(int32 Index)
{
	const TArray<FInputChord> Keys = GetExternalKeys();
	return Keys.IsValidIndex(Index) ? Keys[Index] : FInputChord();
}

FInputChord UVSInputKeyWidgetBinder::GetWidgetKeyAtIndex(int32 Index) const
{
	const TArray<FInputChord> Keys = GetWidgetKeys();
	return Keys.IsValidIndex(Index) ? Keys[Index] : FInputChord();
}

void UVSInputKeyWidgetBinder::RefreshKeys()
{
	
}

void UVSInputKeyWidgetBinder::OnBoundWidgetKeysUpdated()
{
	OnWidgetKeysUpdated(GetWidgetKeys());
	RebindTypedWidget(FName("Keys"));
}

void UVSInputKeyWidgetBinder::OnWidgetKeysUpdated_Implementation(const TArray<FInputChord>& NewKeys)
{
	
}

bool UVSInputKeyWidgetBinder::EditorAllowChangingKeySettings_Implementation() const
{
	return true;
}

void UVSInputKeyWidgetBinder::OnInputKeyGroupUpdated(UVSInputKeySelectorGroupWidget* Group)
{
	OnBoundWidgetKeysUpdated();
}

void UVSInputKeyWidgetBinder::OnContentedSelectorKeySelected(UVSContentedInputKeySelector* Selector, FInputChord SelectedKey)
{
	OnBoundWidgetKeysUpdated();
}

void UVSInputKeyWidgetBinder::OnInputKeySelectorKeySelected(FInputChord SelectedKey)
{
	OnBoundWidgetKeysUpdated();
}
