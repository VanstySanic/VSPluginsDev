// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/VSRangeBasedWidgetBinder.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/VSCommonRanger.h"

UVSRangeBasedWidgetBinder::UVSRangeBasedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSRangeBasedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshRange();

	FInternationalization::Get().OnCultureChanged().AddUObject(this, &UVSRangeBasedWidgetBinder::OnCultureChanged);
}

void UVSRangeBasedWidgetBinder::Uninitialize_Implementation()
{
	FInternationalization::Get().OnCultureChanged().RemoveAll(this);

	Super::Uninitialize_Implementation();
}

void UVSRangeBasedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->SetMinValue(ValueRange.X);
			Slider->SetMaxValue(ValueRange.Y);
			Slider->SetStepSize(StepSize);
			Slider->MouseUsesStep = bSnapByStep;
			Slider->SetValue(GetExternalValue(true));
			
			Slider->OnValueChanged.AddDynamic(this, &UVSRangeBasedWidgetBinder::OnWidgetValueChanged);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			SpinBox->SetMinValue(ValueRange.X);
			SpinBox->SetMaxValue(ValueRange.Y);
			SpinBox->SetDelta(StepSize);
			SpinBox->SetAlwaysUsesDeltaSnap(bSnapByStep);
			
			SpinBox->SetValue(GetExternalValue(true));

			SpinBox->OnValueChanged.AddDynamic(this, &UVSRangeBasedWidgetBinder::OnWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->ValueRange = ValueRange;
			CommonRanger->StepSize = StepSize;
			CommonRanger->bSnapByStep = bSnapByStep;
			CommonRanger->bSupportMutation = bSupportMutation;
			CommonRanger->MutedStateValue = MutedStateValue;
			CommonRanger->RefreshRanger();
			
			CommonRanger->SetValue(GetExternalValue(true), false);
			
			CommonRanger->OnValueChanged_Native.AddUObject(this, &UVSRangeBasedWidgetBinder::OnCommonRangerValueChanged);
		}
	}
	else if (TypeName == FName("Mute"))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->SetIsChecked(GetExternalIsMuted());
			CheckBox->OnCheckStateChanged.AddDynamic(this, &UVSRangeBasedWidgetBinder::OnWidgetMutedStateChanged);
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			const FText& ContentText = GetContentText(GetCurrentValue());
			TextBlock->SetText(ContentText);
		}
	}
}

void UVSRangeBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->OnValueChanged.RemoveDynamic(this, &UVSRangeBasedWidgetBinder::OnWidgetValueChanged);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			SpinBox->OnValueChanged.RemoveDynamic(this, &UVSRangeBasedWidgetBinder::OnWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->OnValueChanged_Native.RemoveAll(this);
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

bool UVSRangeBasedWidgetBinder::GetExternalIsMuted_Implementation() const
{
	return false;
}

float UVSRangeBasedWidgetBinder::GetCurrentValue(bool bIgnoreMutedState) const
{
	if (GetCurrentIsMuted() && !bIgnoreMutedState)
	{
		return MutedStateValue;
	}

	return GetCurrentNonMutedValue();
}

float UVSRangeBasedWidgetBinder::GetCurrentNonMutedValue_Implementation() const
{
	UWidget* RangeWidget = GetBoundTypedWidget(FName("Range"));
	if (!RangeWidget) return GetExternalValue(true);
	
	if (USlider* Slider = Cast<USlider>(RangeWidget))
	{
		return Slider->GetValue();
	}
	if (USpinBox* SpinBox = Cast<USpinBox>(RangeWidget))
	{
		return SpinBox->GetValue();
	}
	if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(RangeWidget))
	{
		return CommonRanger->GetValue(true);
	}

	return GetExternalValue(true);
}

float UVSRangeBasedWidgetBinder::GetExternalValue_Implementation(bool bIgnoreMutedState) const
{
	return !bIgnoreMutedState && GetExternalIsMuted() ? 0.f : 0.f;
}

bool UVSRangeBasedWidgetBinder::GetCurrentIsMuted_Implementation() const
{
	return false;
}

FText UVSRangeBasedWidgetBinder::GetContentText(float Value, bool bMuted, bool bSameValueMutedText) const
{
	if (!DisplayMutedText.IsEmpty())
	{
		if (bMuted)
		{
			return DisplayMutedText;
		}
		if (bSameValueMutedText && FMath::IsNearlyEqual(Value, MutedStateValue))
		{
			return DisplayMutedText;
		}
	}
	
	return FText::Format(DisplayFormatText, Value);
}

void UVSRangeBasedWidgetBinder::RefreshRange()
{
	ValueRange = GenerateValueRange();
	
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Content"));
}

void UVSRangeBasedWidgetBinder::OnWidgetValueChanged_Implementation(float NewValue)
{
	
}

FVector2D UVSRangeBasedWidgetBinder::GenerateValueRange_Implementation() const
{
	return FVector2D::ZeroVector;
}

#if WITH_EDITOR
bool UVSRangeBasedWidgetBinder::EditorAllowChangingStepSize_Implementation() const
{
	return true;
}

bool UVSRangeBasedWidgetBinder::EditorAllowChangingSnapByStep_Implementation() const
{
	return true;
}

bool UVSRangeBasedWidgetBinder::EditorAllowChangingMutedStateValue_Implementation() const
{
	return true;
}

bool UVSRangeBasedWidgetBinder::EditorAllowChangingSupportMutation_Implementation() const
{
	return true;
}
#endif

void UVSRangeBasedWidgetBinder::OnCultureChanged()
{
	RefreshRange();
}

void UVSRangeBasedWidgetBinder::OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value, bool bIsMuteRedirect)
{
	if (!Ranger->IsMuted() || !bIsMuteRedirect
		|| !Ranger->IsMuted() && !bIsMuteRedirect)
	{
		OnBoundWidgetValueChanged(Value);
	}
}

void UVSRangeBasedWidgetBinder::OnWidgetMutedStateChanged(bool bIsMuted)
{
	const bool bPrevIsMuted = bIsMuted;
	bIsMuted = bIsMuted && bSupportMutation;
	if (bPrevIsMuted != bIsMuted)
	{
		OnBoundWidgetValueChanged(bIsMuted);
	}
	else
	{
		RebindWidgetByType(FName("Mute"));
	}
}

void UVSRangeBasedWidgetBinder::OnBoundWidgetValueChanged(float Value)
{
	RebindWidgetByType(FName("Content"));
	OnWidgetValueChanged(Value);
}


