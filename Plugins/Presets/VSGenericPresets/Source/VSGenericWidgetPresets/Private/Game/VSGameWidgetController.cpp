// Copyright VanstySanic. All Rights Reserved.

#include "VSGenericWidgetPresets/Public/Game/VSGameWidgetController.h"
#include "Blueprint/UserWidget.h"
#include "Game/VSGameHUDWidget.h"
#include "Game/VSGameOverlay.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSWidgetTypes.h"

UVSGameWidgetController::UVSGameWidgetController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSGameWidgetController::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	PlayerControllerPrivate = Cast<APlayerController>(UVSGameplayLibrary::GetControllerFromObject(this));
	check(PlayerControllerPrivate.Get());
}

void UVSGameWidgetController::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();

	if (GameHUDWidgetClass)
	{
		GameHUDWidget = CreateWidget<UVSGameHUDWidget>(GetTypedOuter<APlayerController>(), GameHUDWidgetClass);
		check(GameHUDWidget);
		GameHUDWidget->SetOwnerWidgetController(this);
		if (UVSGameOverlay* GameOverlay = GetGameOverlay())
		{
			GameOverlay->SetOwnerWidgetController(this);
		}
		GameHUDWidget->AddToPlayerScreen();
	}
}

void UVSGameWidgetController::EndPlay_Implementation()
{
	if (GameHUDWidget)
	{
		GameHUDWidget->RemoveFromParent();
	}
	
	Super::EndPlay_Implementation();
}

UVSGameOverlay* UVSGameWidgetController::GetGameOverlay() const
{
	return GameHUDWidget ? GameHUDWidget->GetGameOverlay() : nullptr;
}

void UVSGameWidgetController::DisplaySubtitleByParams(const FVSWidgetSubtitleParams& SubtitleParams, FName TypeID)
{
	if (UVSGameOverlay* GameOverlay = GetGameOverlay())
	{
		GameOverlay->DisplaySubtitle(SubtitleParams, TypeID);
	}
}

void UVSGameWidgetController::DisplaySubtitleByRow(const FDataTableRowHandle& SubtitleRow, FName TypeID)
{
	if (FVSWidgetSubtitleParams* SubtitleParams = SubtitleRow.GetRow<FVSWidgetSubtitleParams>(nullptr))
	{
		DisplaySubtitleByParams(*SubtitleParams, TypeID);
	}
}

void UVSGameWidgetController::DisplayNotifyByParams(const FVSWidgetNotifyParams& NotifyParams, FName TypeID)
{
	if (UVSGameOverlay* GameOverlay = GetGameOverlay())
	{
		GameOverlay->DisplayNotify(NotifyParams, TypeID);
	}
}

void UVSGameWidgetController::DisplayNotifyByRow(const FDataTableRowHandle& NotifyRow, FName TypeID)
{
	if (FVSWidgetNotifyParams* NotifyParams = NotifyRow.GetRow<FVSWidgetNotifyParams>(nullptr))
	{
		DisplayNotifyByParams(*NotifyParams, TypeID);
	}
}
