// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreenSubsystemW.h"
#include "GameMapsSettings.h"
#include "VSPrivablic.h"
#include "Kismet/GameplayStatics.h"
#include "Types/VSGameplayTypes.h"
#include "VSSplitScreenSettings.h"
#include "VSSplitScreenSubsystemGI.h"
#include "Classes/Features/VSAlphaBlendFeature.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameViewportClient, SplitscreenInfo, TArray<FSplitscreenData>);

UVSSplitScreenSubsystemW::UVSSplitScreenSubsystemW()
{
	PlayerDatas.Empty();
	PlayerDatas.Emplace(EVSSplitScreenPlayer::PlayerOne, FVSSplitScreenPlayerData());
	PlayerDatas.Emplace(EVSSplitScreenPlayer::PlayerTwo, FVSSplitScreenPlayerData());
}

UVSSplitScreenSubsystemW* UVSSplitScreenSubsystemW::Get(const UObject* WorldContext)
{
	if (!WorldContext || !WorldContext->GetWorld()) return nullptr;
	return WorldContext->GetWorld()->GetSubsystem<UVSSplitScreenSubsystemW>();
}

void UVSSplitScreenSubsystemW::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	bool bIsAvailableMap = false;;
	for (FName SplitScreenMapName : UVSSplitScreenSettings::Get()->SplitScreenMapNames)
	{
		if (SplitScreenMapName == InWorld.GetFName())
		{
			bIsAvailableMap = true;
		}
	}
	if (!bIsAvailableMap) return;
	
	SplitScreenData = &VS_PRIVABLIC_MEMBER(InWorld.GetGameViewport(), UGameViewportClient, SplitscreenInfo)[ESplitScreenType::TwoPlayer_Vertical];
	
	/** Check the split screen settings. */
	check(GetDefault<UGameMapsSettings>()->bUseSplitscreen);
	check(GetDefault<UGameMapsSettings>()->TwoPlayerSplitscreenLayout == ETwoPlayerSplitScreenType::Vertical);
	
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::CreatePlayer(this, 1, false);
		check(GEngine->GetLocalPlayerFromPlatformUserId(&InWorld, FVSUserQueryParams(1).GetUserId()));
		
		if (AGameModeBase* GameModeBase = InWorld.GetAuthGameMode())
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;
			
			LocalController_PlayerID1 = GetWorld()->SpawnActor<APlayerController>(GameModeBase->PlayerControllerClass, SpawnInfo);
			LocalController_PlayerID1->SetPlayer(GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId()));
		}
	}
}

void UVSSplitScreenSubsystemW::InitializeSplitScreen()
{
	UWorld* World = GetWorld();

	bool bIsAvailableMap = false;;
	for (FName SplitScreenMapName : UVSSplitScreenSettings::Get()->SplitScreenMapNames)
	{
		if (SplitScreenMapName == World->GetFName())
		{
			bIsAvailableMap = true;
		}
	}
	if (!bIsAvailableMap) return;
	
	if (World->GetNetMode() != NM_Standalone)
	{
		/** Create player controller for player two. */
		if (!GEngine->GetLocalPlayerFromPlatformUserId(World, FVSUserQueryParams(1).GetUserId()))
		{
			UGameplayStatics::CreatePlayer(this, 1, false);
			check(GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId()));
			
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;
			
			LocalController_PlayerID1 = World->SpawnActor<APlayerController>(UVSSplitScreenSettings::Get()->SplitScreenLocalPlayerControllerClass.LoadSynchronous(), SpawnInfo);
			LocalController_PlayerID1->SetReplicates(false);
			LocalController_PlayerID1->SetPlayer(GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId()));

			/** Assign a controller and an empty pawn for non-standalone generated local player. */
			APawn* LocalPawn = World->SpawnActor<APawn>(APawn::StaticClass(), SpawnInfo);
			LocalPawn->SetReplicates(false);
			LocalController_PlayerID1->Possess(LocalPawn);
			check(LocalController_PlayerID1);
		}
	}
	
	EVSSplitScreenPlayer::Type PlayerLocalID0 = UVSSplitScreenSubsystemGI::Get(this)->GetPlayerFormID(0);
	EVSSplitScreenPlayer::Type PlayerLocalID1 = UVSSplitScreenSubsystemGI::Get(this)->GetPlayerFormID(1);
	
	PlayerDatas[PlayerLocalID0].LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams((int32)0).GetUserId());
	PlayerDatas[PlayerLocalID1].LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId());
	
	/** Only the second local controller is valid, cause the first one may be on replication. */
	PlayerDatas[PlayerLocalID0].LocalController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerDatas[PlayerLocalID1].LocalController = LocalController_PlayerID1;
	
	/** Register the feature. */
	PlayerLayoutRatioAlphaBlendFeature = NewObject<UVSAlphaBlendFeature>(this);
	PlayerLayoutRatioAlphaBlendFeature->OnUpdated.AddDynamic(this, &UVSSplitScreenSubsystemW::OnPlayerLayoutRatioAlphaBlendFeatureUpdated);
	PlayerLayoutRatioAlphaBlendFeature->RegisterFeature();
}

void UVSSplitScreenSubsystemW::DeinitializeSplitScreen()
{
	UWorld* World = GetWorld();

	bool bIsAvailableMap = false;;
	for (FName SplitScreenMapName : UVSSplitScreenSettings::Get()->SplitScreenMapNames)
	{
		if (SplitScreenMapName == World->GetFName())
		{
			bIsAvailableMap = true;
		}
	}
	if (!bIsAvailableMap) return;
	
	if (ULocalPlayer* LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(World, FVSUserQueryParams(1).GetUserId()))
	{
		GetWorld()->GetGameInstance()->RemoveLocalPlayer(LocalPlayer);
	}
	if (PlayerLayoutRatioAlphaBlendFeature->IsValidLowLevel())
	{
		PlayerLayoutRatioAlphaBlendFeature->UnregisterFeature();
	}
	FWorldDelegates::OnWorldBeginTearDown.Remove(OnWorldBeginTearDownDelegateHandle);
}

void UVSSplitScreenSubsystemW::SetPlayerState(EVSSplitScreenPlayer::Type Player, APlayerState* PlayerState)
{
	check(PlayerDatas.Contains(Player));
	PlayerDatas[Player].PlayerState = PlayerState;
	if (PlayerState->GetOwningController() && PlayerState->GetOwningController()->IsLocalController())
	{
		PlayerDatas[Player].LocalController = PlayerState->GetPlayerController();
	}
}

FVSSplitScreenPlayerData UVSSplitScreenSubsystemW::GetSplitScreenPlayerData(EVSSplitScreenPlayer::Type Player) const
{
	return PlayerDatas.FindRef(Player);
}

void UVSSplitScreenSubsystemW::SetPlayerLayoutRatioInternal(float InRatio)
{
	UVSSplitScreenSubsystemGI* SubsystemGI = UVSSplitScreenSubsystemGI::Get(this);

	PlayerLayoutRatio = InRatio;

	FSplitscreenData SplitscreenData = GetTwoPlayerVerticalSplitScreenData();

	int32 PlayerOneID = SubsystemGI->GetIDFormPlayer(EVSSplitScreenPlayer::PlayerOne);
	int32 PlayerTwoID = SubsystemGI->GetIDFormPlayer(EVSSplitScreenPlayer::PlayerTwo);

	SplitscreenData.PlayerData[PlayerOneID].OriginX = 0.f;
	SplitscreenData.PlayerData[PlayerOneID].SizeX = PlayerLayoutRatio;

	SplitscreenData.PlayerData[PlayerTwoID].OriginX = PlayerLayoutRatio;
	SplitscreenData.PlayerData[PlayerTwoID].SizeX = 1.f - PlayerLayoutRatio;
	
	SetTwoPlayerVerticalSplitScreenData(SplitscreenData);
}

void UVSSplitScreenSubsystemW::SetPlayerLayoutRatioWithBlend(float InRatio, const FAlphaBlendArgs& AlphaBlendArgs)
{
	BlendStartPlayerLayoutRatio = PlayerLayoutRatio;
	DesiredPlayerLayoutRatio = FMath::Clamp(InRatio, 0.f, 1.f);
	PlayerLayoutRatioAlphaBlendFeature->SetTime(0.f);
	PlayerLayoutRatioAlphaBlendFeature->SetAlphaBlendArgs(AlphaBlendArgs);
	PlayerLayoutRatioAlphaBlendFeature->SetAutoUpdate(true);
}

void UVSSplitScreenSubsystemW::SetPlayerViewTargetWithBlend(EVSSplitScreenPlayer::Type Player, AActor* ViewTarget, const FViewTargetTransitionParams& TransitionParams)
{
	if (APlayerController* PlayerController = PlayerDatas.FindRef(Player).LocalController)
	{
		check(PlayerController->PlayerCameraManager);
		PlayerController->PlayerCameraManager->SetViewTarget(ViewTarget, TransitionParams);
	}
}

void UVSSplitScreenSubsystemW::OnPlayerLayoutRatioAlphaBlendFeatureUpdated(UVSAlphaBlendFeature* Feature, float Alpha, float UpdatedTime)
{
	const float Ratio = FMath::Lerp(BlendStartPlayerLayoutRatio, DesiredPlayerLayoutRatio, Alpha);
	SetPlayerLayoutRatioInternal(Ratio);
}

FSplitscreenData UVSSplitScreenSubsystemW::GetTwoPlayerVerticalSplitScreenData()
{
	return *SplitScreenData;
}

void UVSSplitScreenSubsystemW::SetTwoPlayerVerticalSplitScreenData(const FSplitscreenData& Data)
{
	*SplitScreenData = Data;
}