// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders//VSOptionBasedWidgetBinder.h"
#include "CommonRotator.h"
#include "VSPrivablic.h"
#include "Components/ComboBoxString.h"
#include "Components/RichTextBlock.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/VSCommonButtonGroupWidget.h"
#include "Components/VSCommonRanger.h"
#include "Groups/CommonButtonGroupBase.h"
#include "Runtime/Slate/Private/Widgets/Views/SListPanel.h"

using SComboBoxString = SComboBox<TSharedPtr<FString>>;

VS_DECLARE_PRIVABLIC_MEMBER(UComboBoxString, MyComboBox, TSharedPtr<SComboBox<TSharedPtr<FString>>>);
VS_DECLARE_PRIVABLIC_MEMBER(SComboBoxString, ComboListView, TSharedPtr<SListView<TSharedPtr<FString>>>);
VS_DECLARE_PRIVABLIC_MEMBER(STableViewBase, ItemsPanel, TSharedPtr<SListPanel>);

UVSOptionBasedWidgetBinder::UVSOptionBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bInternalReverseOptions(false)
	, bInternalHideDisabledOptions(true)
{
}

void UVSOptionBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshOptions();

	FInternationalization::Get().OnCultureChanged().AddUObject(this, &UVSOptionBasedWidgetBinder::OnCultureChanged);
}

void UVSOptionBasedWidgetBinder::Uninitialize_Implementation()
{
	FInternationalization::Get().OnCultureChanged().RemoveAll(this);

	Super::Uninitialize_Implementation();
}

void UVSOptionBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	const FString CurrentOption = GetWidgetOption();
	const FString ExternalOption = GetExternalOption();;
	const int32 ExternalIndex = GetOptionIndex(ExternalOption);

	if (TypeName == FName("Options"))
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			ComboBoxString->ClearOptions();
			
			for (const FText& OptionText : OptionTexts)
			{
				ComboBoxString->AddOption(OptionText.ToString());
			}
			
			ComboBoxString->RefreshOptions();
			ComboBoxString->SetSelectedIndex(ExternalIndex);

			ComboBoxString->OnSelectionChanged.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnComboBoxStringValueChanged);
			ComboBoxString->OnOpening.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnComboBoxStringOpening);
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			CommonRotator->ClearSelection();
			CommonRotator->PopulateTextLabels(OptionTexts);
			CommonRotator->SetSelectedItem(ExternalIndex);

			CommonRotator->OnRotatedWithDirection.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnCommonRotatorValueChanged);
			
			/** Common rotator does not support disabled options. Please turn the bInternalHideDisabledOptions on. */
			// bInternalHideDisabledOptions = true;
		}
		else if (UVSCommonButtonGroupWidget* ButtonGroupWidget = Cast<UVSCommonButtonGroupWidget>(Widget))
		{
			ButtonGroupWidget->ButtonActionSettings.Empty();
			ButtonGroupWidget->OverridenButtonNames.Empty();
			ButtonGroupWidget->DefaultDisabledIndexes.Empty();

			for (int i = 0; i < OptionTexts.Num(); ++i)
			{
				ButtonGroupWidget->OverridenButtonNames.Add(OptionTexts[i]);
				if (IsOptionDisabled(GetOptionAtIndex(i)))
				{
					ButtonGroupWidget->DefaultDisabledIndexes.Add(i);
				}
			}

			ButtonGroupWidget->RefreshButtons();
			ButtonGroupWidget->GetButtonGroup()->SelectButtonAtIndex(ExternalIndex);

			ButtonGroupWidget->GetButtonGroup()->OnSelectedButtonBaseChanged.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnButtonGroupValueChanged);
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->SetMinValue(0.f);
			Slider->SetMaxValue(FMath::Max(Options.Num() - 1.f, 0.f));
			Slider->SetStepSize(1.f);
			Slider->MouseUsesStep = true;

			Slider->SetValue(FMath::Max((float)ExternalIndex, 0.f));

			Slider->OnValueChanged.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnSliderValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->ValueRange.X = 0.f;
			CommonRanger->ValueRange.Y = FMath::Max(Options.Num() - 1.f, 0.f);
			CommonRanger->StepSize = 1.f;
			CommonRanger->bSnapByStep = true;

			CommonRanger->RefreshRanger();
			CommonRanger->SetValue(FMath::Max((float)ExternalIndex, 0.f));

			CommonRanger->OnValueChanged.AddDynamic(this, &UVSOptionBasedWidgetBinder::OnCommonRangerValueChanged);
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(GetWidgetOptionText());
		}
		else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget))
		{
			RichTextBlock->SetText(GetWidgetOptionText());
		}
	}
}

void UVSOptionBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Options"))
	{
		if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(Widget))
		{
			ComboBoxString->OnOpening.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnComboBoxStringOpening);
			ComboBoxString->OnSelectionChanged.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnComboBoxStringValueChanged);
			ComboBoxString->ClearOptions();
		}
		else if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(Widget))
		{
			CommonRotator->OnRotatedWithDirection.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnCommonRotatorValueChanged);
			CommonRotator->ClearSelection();
			CommonRotator->PopulateTextLabels({});
		}
		else if (UVSCommonButtonGroupWidget* ButtonGroupWidget = Cast<UVSCommonButtonGroupWidget>(Widget))
		{
			ButtonGroupWidget->GetButtonGroup()->OnSelectedButtonBaseChanged.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnButtonGroupValueChanged);
			ButtonGroupWidget->GetButtonGroup()->SelectButtonAtIndex(INDEX_NONE);
		}
		else if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->OnValueChanged.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnSliderValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->OnValueChanged.RemoveDynamic(this, &UVSOptionBasedWidgetBinder::OnCommonRangerValueChanged);
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

void UVSOptionBasedWidgetBinder::RefreshOptions()
{
	Options = GenerateOptions();
	if (bInternalReverseOptions)
	{
		Algo::Reverse(Options);
	}
	
	DisabledOptions = GenerateDisabledOptions();

	if (bInternalHideDisabledOptions)
	{
		for (const FString& DisabledOption : DisabledOptions)
		{
			Options.Remove(DisabledOption);
		}
	}
	
	OptionTexts.Empty();
	for (const FString& Option : Options)
	{
		OptionTexts.Add(OptionStringToText(Option));
	}
	
	RebindTypedWidget(FName("Options"));
	RebindTypedWidget(FName("Content"));
}

FString UVSOptionBasedWidgetBinder::GetExternalOption_Implementation() const
{
	return FString();
}

FText UVSOptionBasedWidgetBinder::GetWidgetOptionText() const
{
	const FString Option = GetWidgetOption();
	int32 Index = GetOptionIndex(Option);
	if (OptionTexts.IsValidIndex(Index))
	{
		return OptionTexts[Index];
	}
	return OptionStringToText(Option);
}

int32 UVSOptionBasedWidgetBinder::GetWidgetIndex_Implementation() const
{
	UWidget* OptionWidget = GetBoundTypedWidget(FName("Options"));
	if (!OptionWidget) return GetOptionIndex(GetExternalOption());
	
	if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(OptionWidget))
	{
		return ComboBoxString->GetSelectedIndex();
	}
	if (UCommonRotator* CommonRotator = Cast<UCommonRotator>(OptionWidget))
	{
		return CommonRotator->GetSelectedIndex();
	}
	if (UVSCommonButtonGroupWidget* ButtonGroupWidget = Cast<UVSCommonButtonGroupWidget>(OptionWidget))
	{
		return ButtonGroupWidget->GetButtonGroup()->GetSelectedButtonIndex();
	}
	if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(OptionWidget))
	{
		return FMath::RoundToInt(CommonRanger->GetValue());
	}
	if (USlider* Slider = Cast<USlider>(OptionWidget))
	{
		return FMath::RoundToInt(Slider->GetValue());
	}

	return GetOptionIndex(GetExternalOption());
}

FString UVSOptionBasedWidgetBinder::GetWidgetOption() const
{
	return GetOptionAtIndex(GetWidgetIndex());
}

int32 UVSOptionBasedWidgetBinder::GetOptionIndex(const FString& Option) const
{
	return Options.IndexOfByKey(Option);
}

FString UVSOptionBasedWidgetBinder::GetOptionAtIndex(int32 Index) const
{
	if (Options.IsValidIndex(Index))
	{
		return Options[Index];
	}

	return FString();
}

FText UVSOptionBasedWidgetBinder::GetOptionTextAtIndex(int32 Index) const
{
	if (OptionTexts.IsValidIndex(Index))
	{
		return OptionTexts[Index];
	}
	
	return FText::GetEmpty();
}

FText UVSOptionBasedWidgetBinder::GetTextByOption(const FString& Option) const
{
	const int32 OptionIndex = GetOptionIndex(Option);
	return GetOptionTextAtIndex(OptionIndex);
}

bool UVSOptionBasedWidgetBinder::IsOptionDisabled_Implementation(const FString& Option)
{
	return DisabledOptions.Contains(Option);
}

FText UVSOptionBasedWidgetBinder::OptionStringToText_Implementation(const FString& Option) const
{
	return FText::FromString(Option);
}

TArray<FString> UVSOptionBasedWidgetBinder::GenerateOptions_Implementation() const
{
	return TArray<FString>();
}

void UVSOptionBasedWidgetBinder::OnWidgetOptionChanged_Implementation(int32 NewIndex)
{
	
}

TArray<FString> UVSOptionBasedWidgetBinder::GenerateDisabledOptions_Implementation() const
{
	return TArray<FString>();
}

void UVSOptionBasedWidgetBinder::OnCultureChanged()
{
	RefreshOptions();
}

void UVSOptionBasedWidgetBinder::OnBoundWidgetValueChanged(int32 Index)
{
	OnWidgetOptionChanged(Index);
	RebindTypedWidget(FName("Content"));
}

void UVSOptionBasedWidgetBinder::OnComboBoxStringOpening()
{
	/** Disable item widgets in combobox whose option is disabled. */
	if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(GetBoundTypedWidget(FName("Options"))))
	{
		auto ComboBox = VS_PRIVABLIC_MEMBER(ComboBoxString, UComboBoxString, MyComboBox);
		if (!ComboBox.IsValid()) return;
		auto ListView = VS_PRIVABLIC_MEMBER(ComboBox.Get(), SComboBoxString, ComboListView);
		if (!ListView.IsValid()) return;
		auto TableView = VS_PRIVABLIC_MEMBER(ListView.Get(), STableViewBase, ItemsPanel);
		if (!TableView.IsValid()) return;
		
		if (TableView.IsValid() && ListView.IsValid() && GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, TableView, ListView]()
			{
				if (!IsValid(this) || !GetWorld() || !TableView.IsValid() || !ListView.IsValid()) return;
				GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, TableView, ListView]()
				{
					if (!IsValid(this) || !TableView.IsValid() || !ListView.IsValid()) return;
					
					int32 Index = INDEX_NONE;
					TableView.Get()->GetChildren()->ForEachWidget([& , ListView] (SWidget& Widget)
					{
						Index++;
						
						if (ListView->GetItems().IsValidIndex(Index))
						{
							if (IsOptionDisabled(GetOptionAtIndex(Index)))
							{
								Widget.SetEnabled(false);
							}
						}
					});
				}));
			}));
		}
	}
}

void UVSOptionBasedWidgetBinder::OnComboBoxStringValueChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (UComboBoxString* ComboBoxString = Cast<UComboBoxString>(GetBoundTypedWidget("Options")))
	{
		OnBoundWidgetValueChanged(ComboBoxString->GetSelectedIndex());
	}
	else
	{
		for (int32 i = 0; i < OptionTexts.Num(); i++)
        {
        	if (OptionTexts[i].EqualTo(FText::FromString(SelectedItem)))
        	{
        		OnBoundWidgetValueChanged(i);
        		break;
        	}
        }
	}
}

void UVSOptionBasedWidgetBinder::OnCommonRotatorValueChanged(int32 Value, ERotatorDirection RotatorDir)
{
	OnBoundWidgetValueChanged(Value);
}

void UVSOptionBasedWidgetBinder::OnButtonGroupValueChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex)
{
	OnBoundWidgetValueChanged(ButtonIndex);
}

void UVSOptionBasedWidgetBinder::OnSliderValueChanged(float Value)
{
	OnBoundWidgetValueChanged(FMath::RoundToInt(Value));
}

void UVSOptionBasedWidgetBinder::OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value)
{
	OnBoundWidgetValueChanged(FMath::RoundToInt(Value));
}
