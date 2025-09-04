// Copyright VanstySanic. All Rights Reserved.

#include "VSWidgetUtils.h"
#include "VSPrivablic.h"
#include "Input/CommonUIActionRouterBase.h"

VS_DECLARE_PRIVABLIC_MEMBER(UCommonUIActionRouterBase, ActiveInputConfig, TOptional<FUIInputConfig>);

UVSWidgetUtils::UVSWidgetUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FUIInputConfig UVSWidgetUtils::GetCurrentUIInputConfig(APlayerController* PlayerController)
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

void UVSWidgetUtils::SetCurrentUIInputConfig(APlayerController* PlayerController, const FUIInputConfig& NewConfig)
{
	if (!PlayerController) return;
	if (UCommonUIActionRouterBase* Subsystem = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->SetActiveUIInputConfig(NewConfig, nullptr);
	}
}
