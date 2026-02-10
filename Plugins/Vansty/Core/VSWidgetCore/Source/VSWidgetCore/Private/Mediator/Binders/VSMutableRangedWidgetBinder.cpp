// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders//VSMutableRangedWidgetBinder.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/VSCommonMutableRanger.h"
#include "Kismet/KismetTextLibrary.h"

UVSMutableRangedWidgetBinder::UVSMutableRangedWidgetBinder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSMutableRangedWidgetBinder::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	RefreshRange();

	FInternationalization::Get().OnCultureChanged().AddUObject(this, &UVSMutableRangedWidgetBinder::OnCultureChanged);
}

void UVSMutableRangedWidgetBinder::Uninitialize_Implementation()
{
	FInternationalization::Get().OnCultureChanged().RemoveAll(this);

	Super::Uninitialize_Implementation();
}

void UVSMutableRangedWidgetBinder::BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	Super::BindTypedWidget_Implementation(TypeName, Widget);

	const float ExternalNonMutedValue = GetExternalNonMutedValue();
	const float CurrentNonMutedValue = GetCurrentNonMutedValue();
	const bool bCurrentIsMuted = GetCurrentIsMuted();
	const bool bExternalIsMuted = GetExternalIsMuted();

	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->SetMinValue(ValueRange.X * DisplayValueMultiplier);
			Slider->SetMaxValue(ValueRange.Y * DisplayValueMultiplier);
			Slider->SetStepSize(StepSize * DisplayValueMultiplier);
			Slider->MouseUsesStep = bSnapByStep;
			Slider->SetValue(ExternalNonMutedValue * DisplayValueMultiplier);
			
			Slider->OnValueChanged.AddDynamic(this, &UVSMutableRangedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			SpinBox->SetMinValue(ValueRange.X * DisplayValueMultiplier);
			SpinBox->SetMaxValue(ValueRange.Y * DisplayValueMultiplier);
			SpinBox->SetDelta(StepSize * DisplayValueMultiplier);
			SpinBox->SetMinFractionalDigits(DisplayFractionDigitRange.X);
			SpinBox->SetMaxFractionalDigits(DisplayFractionDigitRange.Y);
			SpinBox->SetAlwaysUsesDeltaSnap(bSnapByStep);
			
			SpinBox->SetValue((float)INT32_MAX);
			SpinBox->SetValue(ExternalNonMutedValue * DisplayValueMultiplier);
			
			SpinBox->OnValueChanged.AddDynamic(this, &UVSMutableRangedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->ValueRange = ValueRange;
			CommonRanger->StepSize = StepSize;
			CommonRanger->bSnapByStep = bSnapByStep;
			
			CommonRanger->DisplayFractionDigitRange = DisplayFractionDigitRange;
			CommonRanger->DisplayTextFormat = DisplayTextFormat;
			CommonRanger->DisplayValueMultiplier = DisplayValueMultiplier;
			
			if (UVSCommonMutableRanger* MutableRanger = Cast<UVSCommonMutableRanger>(Widget))
			{
				MutableRanger->MuteStateValue = MuteStateValue;
				MutableRanger->DisplayMutedText = DisplayMutedText;
			}

			CommonRanger->RefreshRanger();
			CommonRanger->SetValue(ExternalNonMutedValue);
			if (UVSCommonMutableRanger* MutableRanger = Cast<UVSCommonMutableRanger>(Widget))
			{
				MutableRanger->SetIsMuted(bCurrentIsMuted);
			}
			
			CommonRanger->OnValueChanged_Native.AddUObject(this, &UVSMutableRangedWidgetBinder::OnCommonRangerValueChanged);
			if (UVSCommonMutableRanger* MutableRanger = Cast<UVSCommonMutableRanger>(Widget))
			{
				MutableRanger->OnMuteStateChanged_Native.AddUObject(this, &UVSMutableRangedWidgetBinder::OnCommonRangerMuteStateChanged);
			}
		}
	}
	else if (TypeName == FName("Mute"))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->SetIsChecked(bExternalIsMuted);
			CheckBox->OnCheckStateChanged.AddDynamic(this, &UVSMutableRangedWidgetBinder::OnBoundWidgetMuteStateChanged);
		}
	}
	else if (TypeName == FName("Content"))
	{
		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			const FText ContentText = GetContentText();
			TextBlock->SetText(ContentText);
		}
	}
}

void UVSMutableRangedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->OnValueChanged.RemoveDynamic(this, &UVSMutableRangedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			SpinBox->OnValueChanged.RemoveDynamic(this, &UVSMutableRangedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->OnValueChanged_Native.RemoveAll(this);

			if (UVSCommonMutableRanger* MutableRanger = Cast<UVSCommonMutableRanger>(Widget))
			{
				MutableRanger->OnMuteStateChanged_Native.RemoveAll(this);
			}
		}
	}
	else if (TypeName == FName("Mute"))
	{
		if (UCheckBox* CheckBox = Cast<UCheckBox>(Widget))
		{
			CheckBox->OnCheckStateChanged.RemoveDynamic(this, &UVSMutableRangedWidgetBinder::OnBoundWidgetMuteStateChanged);
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

float UVSMutableRangedWidgetBinder::GetExternalValue() const
{
	return GetExternalIsMuted() ? MuteStateValue : GetExternalNonMutedValue();
}

float UVSMutableRangedWidgetBinder::GetCurrentValue() const
{
	return GetCurrentIsMuted() ? MuteStateValue : GetCurrentNonMutedValue();
}

bool UVSMutableRangedWidgetBinder::GetExternalIsMuted_Implementation() const
{
	return false;
}

float UVSMutableRangedWidgetBinder::GetCurrentNonMutedValue_Implementation() const
{
	UWidget* RangeWidget = GetBoundTypedWidget(FName("Range"));
	if (!RangeWidget) return GetExternalNonMutedValue();
	
	if (USlider* Slider = Cast<USlider>(RangeWidget))
	{
		return Slider->GetValue() / DisplayValueMultiplier;
	}
	if (USpinBox* SpinBox = Cast<USpinBox>(RangeWidget))
	{
		return SpinBox->GetValue() / DisplayValueMultiplier;
	}
	if (UVSCommonMutableRanger* CommonRanger = Cast<UVSCommonMutableRanger>(RangeWidget))
	{
		return CommonRanger->UVSCommonRanger::GetValue();
	}

	return GetExternalNonMutedValue();
}

bool UVSMutableRangedWidgetBinder::GetCurrentIsMuted_Implementation() const
{
	UWidget* RangeWidget = GetBoundTypedWidget(FName("Range"));

	if (UVSCommonMutableRanger* CommonRanger = Cast<UVSCommonMutableRanger>(RangeWidget))
	{
		return CommonRanger->GetIsMuted();
	}

	UWidget* MuteWidget = GetBoundTypedWidget(FName("Mute"));
	if (!MuteWidget) return GetExternalIsMuted();

	if (UCheckBox* CheckBox = Cast<UCheckBox>(MuteWidget))
	{
		return CheckBox->IsChecked();
	}

	
	return GetExternalIsMuted();
}

FText UVSMutableRangedWidgetBinder::GetContentText() const
{
	if (GetCurrentIsMuted())
	{
		if (!DisplayMutedText.IsEmpty())
		{
			return DisplayMutedText;
		}
	}
	
	const FText ValueText = UKismetTextLibrary::Conv_DoubleToText(
		GetCurrentValue() * DisplayValueMultiplier, HalfToZero,
		false, true,
		1, 324,
		DisplayFractionDigitRange.X, DisplayFractionDigitRange.Y);

	return FText::Format(DisplayTextFormat, ValueText);
}

void UVSMutableRangedWidgetBinder::RefreshRange()
{
	ValueRange = GenerateValueRange();
	MuteStateValue = GenerateMuteStateValue();
	
	RebindWidgetByType(FName("Range"));
	RebindWidgetByType(FName("Mute"));
	RebindWidgetByType(FName("Content"));
}

float UVSMutableRangedWidgetBinder::GenerateMuteStateValue_Implementation() const
{
	return 0.f;
}

float UVSMutableRangedWidgetBinder::GetExternalNonMutedValue_Implementation() const
{
	return 0.f;
}

void UVSMutableRangedWidgetBinder::OnWidgetMuteStateChanged_Implementation(bool bMuted)
{
	
}

void UVSMutableRangedWidgetBinder::OnWidgetValueChanged_Implementation(float NewValue)
{
	
}

FVector2D UVSMutableRangedWidgetBinder::GenerateValueRange_Implementation() const
{
	return FVector2D::ZeroVector;
}

#if WITH_EDITOR
bool UVSMutableRangedWidgetBinder::EditorAllowChangingStepSize_Implementation() const
{
	return true;
}

bool UVSMutableRangedWidgetBinder::EditorAllowChangingSnapByStep_Implementation() const
{
	return true;
}
#endif

void UVSMutableRangedWidgetBinder::OnCultureChanged()
{
	RefreshRange();
}

void UVSMutableRangedWidgetBinder::OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value)
{
	OnBoundWidgetValueChanged(Value);
}

void UVSMutableRangedWidgetBinder::OnCommonRangerMuteStateChanged(UVSCommonMutableRanger* Ranger, bool bMuted)
{
	OnBoundWidgetMuteStateChanged(bMuted);
}

void UVSMutableRangedWidgetBinder::OnDisplayWidgetValueChanged(float Value)
{
	OnBoundWidgetValueChanged(Value / DisplayValueMultiplier);
}

void UVSMutableRangedWidgetBinder::OnBoundWidgetValueChanged(float Value)
{
	if (FMath::IsNearlyEqual(Value, MuteStateValue))
	{
		RebindWidgetByType(FName("Mute"));
	}
	
	RebindWidgetByType(FName("Content"));
	OnWidgetValueChanged(Value);
}

void UVSMutableRangedWidgetBinder::OnBoundWidgetMuteStateChanged(bool bMuted)
{
	if (bMuted || !FMath::IsNearlyEqual(GetCurrentNonMutedValue(), MuteStateValue))
	{
		OnWidgetMuteStateChanged(bMuted);
	}
	else
	{
		RebindWidgetByType(FName("Mute"));

		if (UVSCommonMutableRanger* Ranger = Cast<UVSCommonMutableRanger>(GetBoundTypedWidget(FName("Range"))))
		{
			Ranger->SetIsMuted(true);
			RebindWidgetByType(FName("Range"));
		}
	}
}
