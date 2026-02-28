// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders//VSRangeBasedWidgetBinder.h"
#include "Components/RichTextBlock.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/VSCommonRanger.h"
#include "Kismet/KismetTextLibrary.h"
#include "Types/Math/VSMath.h"

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

	const float ExternalValue = GetExternalValue();

	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->SetMinValue(ValueRange.X * DisplayValueMultiplier);
			Slider->SetMaxValue(ValueRange.Y * DisplayValueMultiplier);
			Slider->SetStepSize(StepSize * DisplayValueMultiplier);
			Slider->MouseUsesStep = bSnapByStep;
			Slider->SetValue(ExternalValue * DisplayValueMultiplier);
			
			Slider->OnValueChanged.AddDynamic(this, &UVSRangeBasedWidgetBinder::OnDisplayWidgetValueChanged);
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
			SpinBox->SetValue(ExternalValue * DisplayValueMultiplier);
			
			SpinBox->OnValueChanged.AddDynamic(this, &UVSRangeBasedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->ValueRange = ValueRange;
			CommonRanger->StepSize = StepSize;
			CommonRanger->bSnapByStep = bSnapByStep;
			
			CommonRanger->DisplayFractionDigitRange = DisplayFractionDigitRange;
			CommonRanger->DisplayTextFormat = DisplayTextFormat;
			CommonRanger->DisplayValueMultiplier = DisplayValueMultiplier;

			CommonRanger->RefreshRanger();
			CommonRanger->SetValue(ExternalValue);
			
			CommonRanger->OnValueChanged_Native.AddUObject(this, &UVSRangeBasedWidgetBinder::OnCommonRangerValueChanged);
		}
	}
	else if (TypeName == FName("Content"))
	{
		const FText ContentText = GetContentText();

		if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
		{
			TextBlock->SetText(ContentText);
		}
		else if (URichTextBlock* RichTextBlock = Cast<URichTextBlock>(Widget))
		{
			RichTextBlock->SetText(ContentText);
		}
	}
}

void UVSRangeBasedWidgetBinder::UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget)
{
	if (TypeName == FName("Range"))
	{
		if (USlider* Slider = Cast<USlider>(Widget))
		{
			Slider->OnValueChanged.RemoveDynamic(this, &UVSRangeBasedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (USpinBox* SpinBox = Cast<USpinBox>(Widget))
		{
			SpinBox->OnValueChanged.RemoveDynamic(this, &UVSRangeBasedWidgetBinder::OnDisplayWidgetValueChanged);
		}
		else if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(Widget))
		{
			CommonRanger->OnValueChanged_Native.RemoveAll(this);
		}
	}
	
	Super::UnbindTypedWidget_Implementation(TypeName, Widget);
}

float UVSRangeBasedWidgetBinder::GetExternalValue_Implementation() const
{
	return 0.f;
}

float UVSRangeBasedWidgetBinder::GetWidgetValue() const
{
	UWidget* RangeWidget = GetBoundTypedWidget(FName("Range"));
	if (!RangeWidget) return GetExternalValue();
	
	if (USlider* Slider = Cast<USlider>(RangeWidget))
	{
		return FVSMath::SafeDivide(Slider->GetValue(), DisplayValueMultiplier);
	}
	if (USpinBox* SpinBox = Cast<USpinBox>(RangeWidget))
	{
		return FVSMath::SafeDivide(SpinBox->GetValue(), DisplayValueMultiplier);
	}
	if (UVSCommonRanger* CommonRanger = Cast<UVSCommonRanger>(RangeWidget))
	{
		return CommonRanger->GetValue();
	}

	return GetExternalValue();
}

FText UVSRangeBasedWidgetBinder::GetContentText() const
{
	const FText ValueText = UKismetTextLibrary::Conv_DoubleToText(
		GetWidgetValue() * DisplayValueMultiplier, HalfToZero,
		false, true,
		1, 324,
		DisplayFractionDigitRange.X, DisplayFractionDigitRange.Y);

	return FText::Format(DisplayTextFormat, ValueText);
}

void UVSRangeBasedWidgetBinder::RefreshRange()
{
	ValueRange = GenerateValueRange();
	
	RebindTypedWidget(FName("Range"));
	RebindTypedWidget(FName("Content"));
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
#endif

void UVSRangeBasedWidgetBinder::OnCultureChanged()
{
	RebindTypedWidget(FName("Content"));
}

void UVSRangeBasedWidgetBinder::OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value)
{
	OnBoundWidgetValueChanged(Value);
}

void UVSRangeBasedWidgetBinder::OnDisplayWidgetValueChanged(float Value)
{
	OnBoundWidgetValueChanged(FVSMath::SafeDivide(Value, DisplayValueMultiplier));
}

void UVSRangeBasedWidgetBinder::OnBoundWidgetValueChanged(float Value)
{
	OnWidgetValueChanged(Value);
	RebindTypedWidget(FName("Content"));
}

