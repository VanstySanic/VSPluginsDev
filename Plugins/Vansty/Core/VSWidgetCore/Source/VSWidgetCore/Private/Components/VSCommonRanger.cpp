// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonRanger.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

UVSCommonRanger::UVSCommonRanger(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bSnapByStep(false)
{
	bLocked = true;
}

void UVSCommonRanger::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!bDifferRefreshment
#if WITH_EDITOR
		|| IsDesignTime()
#endif
		)
	{
		RefreshRanger();
	}
	
#if WITH_EDITOR
	if (IsDesignTime())
	{
		SetValue(EditorPreviewValue);
	}
#endif

	FInternationalization::Get().OnCultureChanged().AddUObject(this, &UVSCommonRanger::OnCultureChanged);;
}

void UVSCommonRanger::NativeDestruct()
{
	FInternationalization::Get().OnCultureChanged().RemoveAll(this);

	Super::NativeDestruct();
}

bool UVSCommonRanger::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!bValueDelegatesBound)
	{
		bValueDelegatesBound = true;
		if (Slider)
		{
			Slider->OnValueChanged.AddUniqueDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
		if (SpinBox)
		{
			SpinBox->OnValueChanged.AddUniqueDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
	}
	
	return true;
}

FNavigationReply UVSCommonRanger::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	FNavigationReply Reply = Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
	float PrevValue = CurrentValue;
	float NewValue = PrevValue;

	const float StepSizeToUse = StepSize > 0.f ? StepSize : FMath::Max(ValueRange.Y, ValueRange.X) - FMath::Min(ValueRange.Y, ValueRange.X);
	
	if (Slider)
	{
		if (Slider->GetOrientation() == Orient_Horizontal)
		{
			if (InNavigationEvent.GetNavigationType() == EUINavigation::Left)
			{
				NewValue -= StepSizeToUse;
				Reply = FNavigationReply::Stop();
			}
			else if (InNavigationEvent.GetNavigationType() == EUINavigation::Right)
			{
				NewValue += StepSizeToUse;
				Reply = FNavigationReply::Stop();
			}
		}
		else
		{
			if (InNavigationEvent.GetNavigationType() == EUINavigation::Down)
			{
				NewValue -= StepSizeToUse;
				Reply = FNavigationReply::Stop();
			}
			else if (InNavigationEvent.GetNavigationType() == EUINavigation::Up)
			{
				NewValue += StepSizeToUse;
				Reply = FNavigationReply::Stop();
			}
		}
	}
	else if (SpinBox)
	{
		if (InNavigationEvent.GetNavigationType() == EUINavigation::Left)
		{
			NewValue -= StepSizeToUse;
			Reply = FNavigationReply::Stop();
		}
		else if (InNavigationEvent.GetNavigationType() == EUINavigation::Right)
		{
			NewValue += StepSizeToUse;
			Reply = FNavigationReply::Stop();
		}
	}

	if (!FMath::IsNearlyEqual(PrevValue, NewValue))
	{
		SetValue(NewValue);
	}
	
	return Reply;
}

void UVSCommonRanger::SetValue(float NewValue)
{
	const float PrevValue = CurrentValue;
	CurrentValue = FMath::Clamp(NewValue, ValueRange.X, ValueRange.Y);

#if WITH_EDITORONLY_DATA
	EditorPreviewValue = CurrentValue;
#endif
	
	if (!FMath::IsNearlyEqual(PrevValue, CurrentValue))
	{
		OnValueChangedInternal();
	}
}

FText UVSCommonRanger::GetContentText() const
{
	const FText ValueText = UKismetTextLibrary::Conv_DoubleToText(
		CurrentValue * DisplayValueMultiplier, HalfToZero,
		false, true,
		1, 324,
		DisplayFractionDigitRange.X, DisplayFractionDigitRange.Y);
	
	return FText::Format(DisplayTextFormat, ValueText);
}

void UVSCommonRanger::RefreshRanger()
{
	const float StepSizeToUse = StepSize > 0.f ? StepSize : FMath::Max(ValueRange.Y, ValueRange.X) - FMath::Min(ValueRange.Y, ValueRange.X);

	const bool bPrevValueDelegatesBound = bValueDelegatesBound;
	bValueDelegatesBound = false;
	if (Slider)
	{
		if (bPrevValueDelegatesBound)
		{
			Slider->OnValueChanged.RemoveDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
		
		Slider->SetMinValue(ValueRange.X * DisplayValueMultiplier);
		Slider->SetMaxValue(ValueRange.Y * DisplayValueMultiplier);
		Slider->SetStepSize(StepSizeToUse * DisplayValueMultiplier);
		Slider->MouseUsesStep = bSnapByStep;
		
		Slider->SetValue(CurrentValue * DisplayValueMultiplier);

		if (bPrevValueDelegatesBound)
		{
			Slider->OnValueChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
	}
	if (SpinBox)
	{
		if (bPrevValueDelegatesBound)
		{
			SpinBox->OnValueChanged.RemoveDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
		
		SpinBox->SetMinValue(ValueRange.X * DisplayValueMultiplier);
		SpinBox->SetMinSliderValue(ValueRange.X * DisplayValueMultiplier);
		SpinBox->SetMaxValue(ValueRange.Y * DisplayValueMultiplier);
		SpinBox->SetMaxSliderValue(ValueRange.Y * DisplayValueMultiplier);
		SpinBox->SetDelta(StepSizeToUse * DisplayValueMultiplier);
		SpinBox->SetMinFractionalDigits(DisplayFractionDigitRange.X);
		SpinBox->SetMaxFractionalDigits(DisplayFractionDigitRange.Y);
		SpinBox->SetAlwaysUsesDeltaSnap(bSnapByStep);
		
		SpinBox->SetValue((float)INT32_MAX);
		SpinBox->SetValue(CurrentValue * DisplayValueMultiplier);

		if (bPrevValueDelegatesBound)
		{
			SpinBox->OnValueChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
		}
	}
	bValueDelegatesBound = bPrevValueDelegatesBound;
	
	SetValue(CurrentValue);
	RefreshContentText();
}

void UVSCommonRanger::OnValueChangedInternal()
{
	const float Value = CurrentValue;
	
	if (Slider)
	{
		Slider->SetValue(Value * DisplayValueMultiplier);
	}
	if (SpinBox)
	{
		SpinBox->SetValue(Value * DisplayValueMultiplier);
	}
	
	RefreshContentText();
	
	OnValueChanged_Native.Broadcast(this, Value);
	OnValueChanged.Broadcast(this, Value);
}

void UVSCommonRanger::RefreshContentText()
{
	if (TextBlock_Content)
	{
		const FText ValueText = GetContentText();
		TextBlock_Content->SetText(ValueText);
	}
}

void UVSCommonRanger::OnWidgetValueChanged(float Value)
{
	Value /= DisplayValueMultiplier;
	if (!FMath::IsNearlyEqual(Value, CurrentValue))
	{
		const bool bPrevDelegatesBound = bValueDelegatesBound;
		if (bPrevDelegatesBound)
		{
			bValueDelegatesBound = false;
			if (Slider)
			{
				Slider->OnValueChanged.RemoveDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
			}
			if (SpinBox)
			{
				SpinBox->OnValueChanged.RemoveDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
			}
		}
		
		SetValue(Value);

		if (bPrevDelegatesBound)
		{
			bValueDelegatesBound = true;
			if (Slider)
			{
				Slider->OnValueChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
			}
			if (SpinBox)
			{
				SpinBox->OnValueChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetValueChanged);
			}
		}
	}
}

void UVSCommonRanger::OnCultureChanged()
{
	if (TextBlock_Content)
	{
		TextBlock_Content->SetText(GetContentText());
	}
}
