// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonRanger.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"

UVSCommonRanger::UVSCommonRanger(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bSnapByStep(false)
	, bSupportMutation(false)
	, bDisplaySameValueMutedText(true)
{
	bLocked = true;
}

void UVSCommonRanger::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	LastRefreshedMutedValue = MutedStateValue;
	
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
		SetIsMuted(EditorPreviewMuteState);
	}
#endif
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
	if (!bMuteDelegateBound && CheckBox_Mute)
	{
		bMuteDelegateBound = true;
		CheckBox_Mute->OnCheckStateChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetMuteStateChanged);
	}
	
	return true;
}

FNavigationReply UVSCommonRanger::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	FNavigationReply Reply = Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
	float PrevValue = GetValue(true);
	float NewValue = PrevValue;

	const float StepSizeToUse = FMath::Max(StepSize, FMath::Max(ValueRange.Y, ValueRange.X) - FMath::Min(ValueRange.Y, ValueRange.X));
	
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

	if (PrevValue != NewValue)
	{
		SetValue(NewValue);
	}
	
	return Reply;
}

void UVSCommonRanger::NativeOnClicked()
{
	Super::NativeOnClicked();

	SetIsMuted(!IsMuted());
}

void UVSCommonRanger::SetValue(float NewValue, bool bIsMutedValue)
{
	if (!bIsMutedValue)
	{
		const float PrevNonMuteValue = GetValue(true);
        CurrentNonMuteValue = NewValue;
		
#if WITH_EDITORONLY_DATA
		EditorPreviewValue = CurrentNonMuteValue;
#endif
		
        if (!FMath::IsNearlyEqual(CurrentNonMuteValue, PrevNonMuteValue))
        {
        	OnValueChangedInternal(NewValue, false);
        }
	}
	else
	{
		NewValue = FMath::Clamp(NewValue, ValueRange.X, ValueRange.Y);
		const float PrevNonMuteValue = MutedStateValue;
		MutedStateValue = NewValue;
		if (!FMath::IsNearlyEqual(MutedStateValue, PrevNonMuteValue))
		{
			LastRefreshedMutedValue = MutedStateValue;
			OnValueChangedInternal(NewValue, true);
		}
	}
}

float UVSCommonRanger::GetValue(bool bIgnoreMuteState) const
{
	return !bIgnoreMuteState && bIsMuted ? MutedStateValue : CurrentNonMuteValue;
}

FText UVSCommonRanger::GetContentText(float Value, bool bMuted, bool bSameValueMutedText) const
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

void UVSCommonRanger::RefreshRanger()
{
	if (Slider)
	{
		Slider->SetMinValue(ValueRange.X);
		Slider->SetMaxValue(ValueRange.Y);
		Slider->SetStepSize(StepSize);
		Slider->MouseUsesStep = bSnapByStep;
		
		Slider->SetValue(CurrentNonMuteValue);
	}

	if (SpinBox)
	{
		SpinBox->SetMinValue(ValueRange.X);
		SpinBox->SetMaxValue(ValueRange.Y);
		SpinBox->SetDelta(StepSize);
		SpinBox->SetAlwaysUsesDeltaSnap(bSnapByStep);
		
		SpinBox->SetValue(CurrentNonMuteValue);
	}

	if (CheckBox_Mute)
	{
		CheckBox_Mute->SetIsChecked(bIsMuted && bSupportMutation);
	}

	SetValue(CurrentNonMuteValue, false);
	if (!FMath::IsNearlyEqual(LastRefreshedMutedValue, MutedStateValue))
	{
		OnValueChangedInternal(MutedStateValue, true);
	}
	RefreshContentText();
}

void UVSCommonRanger::SetIsMuted(bool bMuted)
{
	bMuted = bSupportMutation && bMuted;
	
	const bool bPrevMuted = bIsMuted;
	bIsMuted = bMuted;

#if WITH_EDITORONLY_DATA
	EditorPreviewMuteState = bIsMuted;
#endif

	if (CheckBox_Mute)
	{
		CheckBox_Mute->SetIsChecked(bMuted);
	}

	if (bPrevMuted != bIsMuted)
	{
		RefreshContentText();
	}
}

void UVSCommonRanger::OnValueChangedInternal(float Value, bool bIsMutedValue)
{
	if (!bIsMutedValue)
	{
		if (Slider)
		{
			Slider->SetValue(Value);
		}
		if (SpinBox)
		{
			SpinBox->SetValue(Value);
		}
	}
	else
	{
		LastRefreshedMutedValue = MutedStateValue;
	}
	
	RefreshContentText();
	
	OnValueChanged_Native.Broadcast(this, Value, bIsMutedValue);
	OnValueChanged.Broadcast(this, Value, bIsMutedValue);
}

void UVSCommonRanger::RefreshContentText()
{
	if (TextBlock_Content)
	{
		const float CurrentValue = GetValue(false);
		const FText& ValueText = GetContentText(CurrentValue, bIsMuted, bDisplaySameValueMutedText);
		TextBlock_Content->SetText(ValueText);
	}
}

void UVSCommonRanger::OnWidgetValueChanged(float Value)
{
	if (!FMath::IsNearlyEqual(Value, GetValue(true)))
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
		
		SetValue(Value, false);

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

void UVSCommonRanger::OnWidgetMuteStateChanged(bool bMuted)
{
	bMuted = bMuted && bSupportMutation;
	if (bMuted != bIsMuted)
	{
		if (CheckBox_Mute)
		{
			const bool bPrevDelegateBound = bMuteDelegateBound;
			if (bPrevDelegateBound)
			{
				bMuteDelegateBound = false;
				CheckBox_Mute->OnCheckStateChanged.RemoveDynamic(this, &UVSCommonRanger::OnWidgetMuteStateChanged);
			}

			SetIsMuted(bMuted);

			if (bPrevDelegateBound)
			{
				bMuteDelegateBound = true;
				CheckBox_Mute->OnCheckStateChanged.AddDynamic(this, &UVSCommonRanger::OnWidgetMuteStateChanged);
			}
		}
	}
}
