// Copyright VanstySanic. All Rights Reserved.

#include "Interactive/Feature/VSInteractiveTipWidgetFeature.h"
#include "Components/WidgetComponent.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"

UVSInteractiveTipWidgetFeature::UVSInteractiveTipWidgetFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSInteractiveTipWidgetFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	WidgetComponent = Cast<UWidgetComponent>(UVSActorLibrary::GetActorComponentByName(GetOwnerActor(), TipWidgetComponentName));
	WidgetComponent->SetVisibility(false);

	GetInteractiveFeatureAgent()->OnInspectionStart.AddDynamic(this, &UVSInteractiveTipWidgetFeature::OnInspectionStart);
	GetInteractiveFeatureAgent()->OnInspectionEnd.AddDynamic(this, &UVSInteractiveTipWidgetFeature::OnInspectionEnd);
	GetInteractiveFeatureAgent()->OnInteractionStart.AddDynamic(this, &UVSInteractiveTipWidgetFeature::OnInteractionStart);
}

void UVSInteractiveTipWidgetFeature::Uninitialize_Implementation()
{
	if (UVSInteractiveFeatureAgent* Agent = GetInteractiveFeatureAgent())
	{
		Agent->OnInspectionStart.RemoveDynamic(this, &UVSInteractiveTipWidgetFeature::OnInspectionStart);
		Agent->OnInspectionStart.RemoveDynamic(this, &UVSInteractiveTipWidgetFeature::OnInspectionEnd);
		Agent->OnInteractionStart.RemoveDynamic(this, &UVSInteractiveTipWidgetFeature::OnInteractionStart);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSInteractiveTipWidgetFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (WidgetComponent.IsValid())
	{
		TipWidget = WidgetComponent->GetWidget();
		WidgetComponent->SetVisibility(false);
		if (TipWidget.IsValid())
		{
			TipWidget->SetVisibility(ESlateVisibility::Collapsed);
			TipWidget->SetRenderOpacity(0.f);
		}
	}
}

void UVSInteractiveTipWidgetFeature::OnFeatureDeactivated_Implementation()
{
	SetShouldShowTipWidget(false);
	HideTipWidgetImmediately();
	
	Super::OnFeatureDeactivated_Implementation();
}

bool UVSInteractiveTipWidgetFeature::CanTick_Implementation() const
{
	return Super::CanTick_Implementation() && WidgetComponent.IsValid() && TipWidget.IsValid();
}

void UVSInteractiveTipWidgetFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	const bool bSkipBlend = (bShouldShowTipWidget && FMath::IsNearlyEqual(TipWidget->GetRenderOpacity(), 1.f)) || (!bShouldShowTipWidget && FMath::IsNearlyZero(TipWidget->GetRenderOpacity()));
	if (!bSkipBlend && (bWidgetOpacityBlendIn && bShouldShowTipWidget) || (bWidgetOpacityBlendOut && !bShouldShowTipWidget))
	{
		const float DirectionSign = bShouldShowTipWidget ? 1.f : -1.f;
		if (!(DirectionSign > 0.f && FMath::IsNearlyEqual(WidgetOpacityBlendTime, OpacityAlphaBlendArgs.BlendTime)) && !(DirectionSign < 0.f && FMath::IsNearlyZero(WidgetOpacityBlendTime)))
		{
			WidgetOpacityBlendTime = FMath::Clamp(WidgetOpacityBlendTime + DirectionSign * DeltaTime, 0.f, OpacityAlphaBlendArgs.BlendTime);
			const float LinerAlpha = UKismetMathLibrary::MapRangeClamped(WidgetOpacityBlendTime, 0.f, OpacityAlphaBlendArgs.BlendTime, 0.f, 1.f);
			const float OpacityAlpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha, OpacityAlphaBlendArgs.BlendOption, OpacityAlphaBlendArgs.CustomCurve);

			TipWidget->SetRenderOpacity(OpacityAlpha);
		
			if (!bShouldShowTipWidget && FMath::IsNearlyZero(WidgetOpacityBlendTime))
			{
				HideTipWidgetImmediately();
			}
		}
	}

	if (bShouldShowTipWidget && CurrentInteractLocalAgent.IsValid() && InteractionHideTipWidgetDelay > 0.f)
	{
		if (CurrentInteractLocalAgent->GetInteractionTime() >= InteractionHideTipWidgetDelay)
		{
			SetShouldShowTipWidget(false);
			if (bWidgetHideImmediatelyWithInteraction)
			{
				HideTipWidgetImmediately();
			}
		}
	}
}

void UVSInteractiveTipWidgetFeature::SetShouldShowTipWidget(bool bShouldShow)
{
	if (bShouldShowTipWidget == bShouldShow) return;
	bShouldShowTipWidget = bShouldShow;
	if (bShouldShowTipWidget)
	{
		if (!bWidgetOpacityBlendIn)
		{
			ShowTipWidgetImmediately();
		}
		else
		{
			WidgetComponent->SetVisibility(true);
			TipWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (!bShouldShowTipWidget && !bWidgetOpacityBlendIn)
	{
		HideTipWidgetImmediately();
	}
}

void UVSInteractiveTipWidgetFeature::ShowTipWidgetImmediately()
{
	WidgetComponent->SetVisibility(true);
	TipWidget->SetVisibility(ESlateVisibility::Visible);
	TipWidget->SetRenderOpacity(1.f);
	WidgetOpacityBlendTime = OpacityAlphaBlendArgs.BlendTime;
}

void UVSInteractiveTipWidgetFeature::HideTipWidgetImmediately()
{
	if (WidgetComponent.IsValid())
	{
		WidgetComponent->SetVisibility(false);
	}

	if (TipWidget.IsValid())
	{
		TipWidget->SetVisibility(ESlateVisibility::Collapsed);
		TipWidget->SetRenderOpacity(0.f);
	}

	WidgetOpacityBlendTime = 0.f;
}

void UVSInteractiveTipWidgetFeature::OnInspectionStart(UVSInteractFeatureAgent* SourceAgent)
{
	if (SourceAgent && UVSActorLibrary::IsActorLocal(SourceAgent->GetOwnerActor()))
	{
		SetShouldShowTipWidget(true);
	}
}

void UVSInteractiveTipWidgetFeature::OnInspectionEnd(UVSInteractFeatureAgent* SourceAgent)
{
	if (SourceAgent && UVSActorLibrary::IsActorLocal(SourceAgent->GetOwnerActor()))
	{
		if (!SourceAgent->IsInteracting() || SourceAgent->GetCurrentInteractiveFeatureAgent() != GetInteractiveFeatureAgent())
		{
			SetShouldShowTipWidget(false);
		}
	}
}

void UVSInteractiveTipWidgetFeature::OnInteractionStart(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName)
{
	if (SourceAgent && UVSActorLibrary::IsActorLocal(SourceAgent->GetOwnerActor()))
	{
		if (SourceAgent->GetCurrentInteractiveFeatureAgent() == GetInteractiveFeatureAgent())
		{
			CurrentInteractLocalAgent = SourceAgent;
			if (FMath::IsNearlyZero(InteractionHideTipWidgetDelay))
			{
				SetShouldShowTipWidget(false);
				if (bWidgetHideImmediatelyWithInteraction)
				{
					HideTipWidgetImmediately();
				}
			}
		}
		else
		{
			CurrentInteractLocalAgent.Reset();
			if (bWidgetHideImmediatelyWithInteraction)
			{
				HideTipWidgetImmediately();
			}
		}
	}
}
