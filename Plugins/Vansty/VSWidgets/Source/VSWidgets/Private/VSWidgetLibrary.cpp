// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgetLibrary.h"
#include "CommonRotator.h"
#include "VSPrivablic.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Types/VSWidgetTypes.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCommonUIActionRouterBase, ActiveInputConfig, TOptional<FUIInputConfig>);
VS_DECLARE_PRIVABLIC_MEMBER(UCommonRotator, TextLabels, TArray<FText>);

UVSWidgetLibrary::UVSWidgetLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

int32 UVSWidgetLibrary::GetViewportMaxWidgetZOrder(APlayerController* PlayerController)
{
	if (!PlayerController) return INT16_MIN;
	UGameViewportSubsystem* Subsystem = UGameViewportSubsystem::Get(PlayerController->GetWorld());
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(PlayerController, Widgets, UUserWidget::StaticClass());

	int32 MaxWidgetPriority = INT16_MIN;
	for (UUserWidget* Widget : Widgets)
	{
		if (Widget->IsInViewport() && Widget->GetOwningPlayer() == PlayerController)
		{
			FGameViewportWidgetSlot ViewportSlot = Subsystem->GetWidgetSlot(Widget);
			if (ViewportSlot.ZOrder >= MaxWidgetPriority)
			{
				MaxWidgetPriority = ViewportSlot.ZOrder;
			}
		}
	}

	return MaxWidgetPriority;
}

void UVSWidgetLibrary::SetWidgetDesiredFocusedWidget(UUserWidget* Source, UWidget* Desired)
{
	if (!Source || !Desired || !Desired->IsChildOf(Source)) return;
	Source->SetDesiredFocusWidget(Desired);
}

FUIInputConfig UVSWidgetLibrary::GetCurrentUIInputConfig(APlayerController* PlayerController)
{
	if (!PlayerController) return FUIInputConfig();
	if (UCommonUIActionRouterBase* Subsystem = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(PlayerController->GetLocalPlayer()))
	{
		TOptional<FUIInputConfig> ConfigOption = VS_PRIVABLIC_MEMBER(Subsystem, UCommonUIActionRouterBase, ActiveInputConfig);
		if (ConfigOption.IsSet())
		{
			return ConfigOption.GetValue();
		}

		if (UGameViewportClient* GameViewportClient = PlayerController->GetLocalPlayer()->ViewportClient)
		{
			FUIInputConfig Config(Subsystem->GetActiveInputMode(), GameViewportClient->GetMouseCaptureMode(), GameViewportClient->HideCursorDuringCapture());
			Config.bIgnoreMoveInput = PlayerController->IsMoveInputIgnored();
			Config.bIgnoreLookInput = PlayerController->IsLookInputIgnored();
			return Config;
		}
	}
	return FUIInputConfig();
}

void UVSWidgetLibrary::SetCurrentUIInputConfig(APlayerController* PlayerController, const FUIInputConfig& NewConfig)
{
	if (!PlayerController) return;
	if (UCommonUIActionRouterBase* Subsystem = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->SetActiveUIInputConfig(NewConfig, nullptr);
	}
}

void UVSWidgetLibrary::ApplyCommonButtonDisplayParams(UCommonButtonBase* Button, const FVSCommonButtonDisplayParams& Params)
{
	Params.ApplyToButton(Button);
}

void UVSWidgetLibrary::SetCommonRotatorSelectedItemWithNotification(UCommonRotator* Rotator, int32 NewIndex)
{
	if (!Rotator) return;
	const int32 OldIndex = Rotator->GetSelectedIndex();
	if (NewIndex == OldIndex) return;
	
	Rotator->SetSelectedItem(NewIndex);
	
	const int32 OptionNum = GetCommonRotatorOptions(Rotator).Num();
	ERotatorDirection RotatorDirection = ERotatorDirection::Right;
	RotatorDirection = 2 * (( NewIndex > OldIndex ? 0 : OptionNum) + NewIndex - OldIndex) <= OptionNum ? ERotatorDirection::Right : ERotatorDirection::Left;

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	Rotator->OnRotated.Broadcast(NewIndex);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	Rotator->OnRotatedWithDirection.Broadcast(NewIndex, RotatorDirection);
	Rotator->OnRotatedEvent.Broadcast(NewIndex, false);
}

TArray<FText> UVSWidgetLibrary::GetCommonRotatorOptions(UCommonRotator* Rotator)
{
	return VS_PRIVABLIC_MEMBER(Rotator, UCommonRotator, TextLabels);
}
