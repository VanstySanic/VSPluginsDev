// Copyright VanstySanic. All Rights Reserved.

#include "Components/VSCommonMutableRanger.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"

UVSCommonMutableRanger::UVSCommonMutableRanger(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSCommonMutableRanger::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime())
	{
		SetIsMuted(EditorPreviewMuteState);
	}
#endif
}

bool UVSCommonMutableRanger::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!bMuteDelegateBound)
	{
		bMuteDelegateBound = true;

		if (CheckBox_Mute)
		{
			CheckBox_Mute->OnCheckStateChanged.AddUniqueDynamic(this, &UVSCommonMutableRanger::OnWidgetMuteStateChanged);
		}
	}
	
	return true;
}

float UVSCommonMutableRanger::GetValue() const
{
	return bIsMuted ? MuteStateValue : Super::GetValue();
}

void UVSCommonMutableRanger::SetIsMuted(bool bMuted)
{
	const bool bPrevMuted = bIsMuted;

	if (FMath::IsNearlyEqual(Super::GetValue(), MuteStateValue))
	{
		bMuted = true;
	}
	
	bIsMuted = bMuted;

#if WITH_EDITORONLY_DATA
	EditorPreviewMuteState = bIsMuted;
#endif
	
	if (bPrevMuted != bIsMuted)
	{
		OnMuteStateChangedInternal();
	}
}

void UVSCommonMutableRanger::OnWidgetValueChanged(float Value)
{
	const float PrevWidgetValue = Super::GetValue();
	
	Super::OnWidgetValueChanged(Value);

	const float CurrentWidgetValue = Super::GetValue();
	
	if (!FMath::IsNearlyEqual(CurrentWidgetValue, PrevWidgetValue))
	{
		if (FMath::IsNearlyEqual(CurrentWidgetValue, MuteStateValue))
		{
			if (!GetIsMuted())
			{
				SetIsMuted(true);
			}
		}
		else if (GetIsMuted())
		{
			SetIsMuted(false);
		}
	}
}

void UVSCommonMutableRanger::OnValueChangedInternal()
{
	const float NonMutedValue = Super::GetValue();
	if (Slider)
	{
		Slider->SetValue(NonMutedValue * DisplayValueMultiplier);
	}
	if (SpinBox)
	{
		SpinBox->SetValue(NonMutedValue * DisplayValueMultiplier);
	}

	const float Value = GetValue();

#if WITH_EDITORONLY_DATA
	EditorPreviewValueWithMuteState = GetValue();
#endif
	
	RefreshContentText();

	OnNonMutedValueChanged_Native.Broadcast(this, NonMutedValue);
	OnNonMutedValueChanged.Broadcast(this, NonMutedValue);

	/** Force mute when non-muted value reaches configured mute sentinel value. */
	bool bMuteStateChanged = false;
	if (FMath::IsNearlyEqual(NonMutedValue, MuteStateValue))
	{
		if (!GetIsMuted())
		{
			bIsMuted = true;
			
#if WITH_EDITORONLY_DATA
			EditorPreviewMuteState = bIsMuted;
#endif
			
			bMuteStateChanged = true;
			
			OnMuteStateChangedInternal();
		}
	}
	
	if (!bMuteStateChanged)
	{
		OnValueChanged_Native.Broadcast(this, Value);
		OnValueChanged.Broadcast(this, Value);
	}
}

void UVSCommonMutableRanger::RefreshRanger()
{
	Super::RefreshRanger();

	if (CheckBox_Mute)
	{
		CheckBox_Mute->SetIsChecked(bIsMuted);
	}

	if (!FMath::IsNearlyEqual(LastRefreshedMutedValue, MuteStateValue))
	{
		if (GetIsMuted())
		{
#if WITH_EDITORONLY_DATA
			EditorPreviewValueWithMuteState = GetValue();
#endif
		}
	}

	LastRefreshedMutedValue = MuteStateValue;
}

FText UVSCommonMutableRanger::GetContentText() const
{
	if (GetIsMuted())
	{
		if (!DisplayMutedText.IsEmpty())
		{
			return DisplayMutedText;
		}
	}
	
	return Super::GetContentText();
}

void UVSCommonMutableRanger::OnMuteStateChangedInternal()
{
	if (CheckBox_Mute)
	{
		CheckBox_Mute->SetIsChecked(bIsMuted);
	}
	
	RefreshContentText();
	
	OnMuteStateChanged_Native.Broadcast(this, bIsMuted);
	OnMuteStateChanged.Broadcast(this, bIsMuted);

	if (!FMath::IsNearlyEqual(MuteStateValue, Super::GetValue()))
	{
#if WITH_EDITORONLY_DATA
		EditorPreviewValueWithMuteState = GetValue();
#endif
		
		OnValueChanged_Native.Broadcast(this, GetValue());
		OnValueChanged.Broadcast(this, GetValue());
	}
}

void UVSCommonMutableRanger::OnWidgetMuteStateChanged(bool bMuted)
{
	if (bMuted != bIsMuted)
	{
		if (CheckBox_Mute)
		{
			const bool bPrevDelegateBound = bMuteDelegateBound;
			if (bPrevDelegateBound)
			{
				bMuteDelegateBound = false;
				CheckBox_Mute->OnCheckStateChanged.RemoveDynamic(this, &UVSCommonMutableRanger::OnWidgetMuteStateChanged);
			}

			if (FMath::IsNearlyEqual(Super::GetValue(), MuteStateValue))
			{
				CheckBox_Mute->SetIsChecked(bIsMuted);
				SetIsMuted(true);
			}
			else
			{
				SetIsMuted(bMuted);
			}

			if (bPrevDelegateBound)
			{
				bMuteDelegateBound = true;
				CheckBox_Mute->OnCheckStateChanged.AddDynamic(this, &UVSCommonMutableRanger::OnWidgetMuteStateChanged);
			}
		}
	}
}
