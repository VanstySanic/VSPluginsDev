// Copyright VanstySanic. All Rights Reserved.

#include "VSSplitScreenSubsystemW.h"
#include "GameMapsSettings.h"
#include "VSPrivablic.h"
#include "Kismet/GameplayStatics.h"
#include "Types/VSGameplayTypes.h"
#include "VSSplitScreenSettings.h"
#include "VSSplitScreenSubsystemGI.h"
#include "Classes/Features/VSAlphaBlendFeature.h"
#include "GameFramework/PlayerState.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameViewportClient, SplitscreenInfo, TArray<FSplitscreenData>);
VS_DECLARE_PRIVABLIC_MEMBER(UWorld, OnBeginPlay, UWorld::FOnBeginPlay);

UVSSplitScreenSubsystemW::UVSSplitScreenSubsystemW()
{
	// PlayerLayoutRatioAlphaBlendFeature = CreateDefaultSubobject<UVSAlphaBlendFeature>(TEXT("PlayerLayoutRatioAlphaBlendFeature"));
	
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

	/** Check the split screen settings. */
	check(GetDefault<UGameMapsSettings>()->bUseSplitscreen);
	check(GetDefault<UGameMapsSettings>()->TwoPlayerSplitscreenLayout == ETwoPlayerSplitScreenType::Vertical);
	
	bool bIsAvailableMap = false;;
	for (FName SplitScreenMapName : UVSSplitScreenSettings::Get()->SplitScreenMapNames)
	{
		if (SplitScreenMapName == InWorld.GetFName())
		{
			bIsAvailableMap = true;
		}
	}
	if (!bIsAvailableMap) return;

	if (InWorld.IsNetMode(NM_Standalone))
	{
		/** Create a local player if the second one not existing. */
		if (!GEngine->GetLocalPlayerFromPlatformUserId(&InWorld, FVSUserQueryParams(1).GetUserId()))
		{
			UGameplayStatics::CreatePlayer(this, 1, true);
			check(GEngine->GetLocalPlayerFromPlatformUserId(&InWorld, FVSUserQueryParams(1).GetUserId()));
		}
	}

	VS_PRIVABLIC_MEMBER(&InWorld, UWorld, OnBeginPlay).AddLambda([&] (bool bBeginPlay)
	{
		if (!bBeginPlay) return;

		// TODO Do it in another way.
		if (!GetWorld()->IsNetMode(NM_Standalone) && !InWorld.IsNetMode(NM_ListenServer))
		{
			UVSSplitScreenSubsystemGI::Get(this)->SetPlayer(0, EVSSplitScreenPlayer::PlayerTwo);
			UVSSplitScreenSubsystemGI::Get(this)->SetPlayer(1, EVSSplitScreenPlayer::PlayerOne);
		}
		
		EVSSplitScreenPlayer::Type PlayerLocalID0 = UVSSplitScreenSubsystemGI::Get(this)->GetPlayerFormID(0);
		EVSSplitScreenPlayer::Type PlayerLocalID1 = UVSSplitScreenSubsystemGI::Get(this)->GetPlayerFormID(1);
		
		if (!GetWorld()->IsNetMode(NM_Standalone))
		{
			if (!GEngine->GetLocalPlayerFromPlatformUserId(&InWorld, FVSUserQueryParams(1).GetUserId()))
			{
				UGameplayStatics::CreatePlayer(this, 1, false);
				check(GEngine->GetLocalPlayerFromPlatformUserId(&InWorld, FVSUserQueryParams(1).GetUserId()));
			}
		}
		
		PlayerDatas[PlayerLocalID0].LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams((int32)0).GetUserId());
		PlayerDatas[PlayerLocalID1].LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId());
		
		/** Create player controller for player two. */
		if (InWorld.IsNetMode(NM_Standalone))
		{
			LocalController_PlayerID1 = UGameplayStatics::GetPlayerController(this, 1);
		}
		else
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;
			LocalController_PlayerID1 = InWorld.SpawnActor<APlayerController>(UVSSplitScreenSettings::Get()->SplitScreenLocalPlayerControllerClass.LoadSynchronous(), SpawnInfo);
		}
		check(LocalController_PlayerID1);

		/** Assign an empty pawn for non-standalone generated local player. */
		if (!InWorld.IsNetMode(NM_Standalone))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;
			APawn* LocalPawn = InWorld.SpawnActor<APawn>(APawn::StaticClass(), SpawnInfo);
			LocalController_PlayerID1->SetPlayer(GEngine->GetLocalPlayerFromPlatformUserId(GetWorld(), FVSUserQueryParams(1).GetUserId()));
			LocalController_PlayerID1->Possess(LocalPawn);
		}
		
		/** Only the second local controller is valid, cause the first one may be on replication. */
		PlayerDatas[PlayerLocalID0].LocalController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerDatas[PlayerLocalID1].LocalController = LocalController_PlayerID1;
		
		PlayerLayoutRatioAlphaBlendFeature = NewObject<UVSAlphaBlendFeature>(this);
		PlayerLayoutRatioAlphaBlendFeature->OnUpdated.AddDynamic(this, &UVSSplitScreenSubsystemW::OnPlayerLayoutRatioAlphaBlendFeatureUpdated);
		PlayerLayoutRatioAlphaBlendFeature->RegisterFeature();
	});

	OnWorldBeginTearDownDelegateHandle = FWorldDelegates::OnWorldBeginTearDown.AddLambda([&] (UWorld* World)
	{
		if (World != &InWorld) return;
		if (ULocalPlayer* LocalPlayer = GEngine->GetLocalPlayerFromPlatformUserId(World, FVSUserQueryParams(1).GetUserId()))
		{
			World->GetGameInstance()->RemoveLocalPlayer(LocalPlayer);
		}
		if (PlayerLayoutRatioAlphaBlendFeature->IsValidLowLevel())
		{
			PlayerLayoutRatioAlphaBlendFeature->UnregisterFeature();
		}
		FWorldDelegates::OnWorldBeginTearDown.Remove(OnWorldBeginTearDownDelegateHandle);
	});
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

	FSplitscreenData SplitscreenData = GetTwoPlayerVerticalSplitScreenData(this);

	int32 PlayerOneID = SubsystemGI->GetIDFormPlayer(EVSSplitScreenPlayer::PlayerOne);
	int32 PlayerTwoID = SubsystemGI->GetIDFormPlayer(EVSSplitScreenPlayer::PlayerTwo);

	SplitscreenData.PlayerData[PlayerOneID].OriginX = 0.f;
	SplitscreenData.PlayerData[PlayerOneID].SizeX = PlayerLayoutRatio;

	SplitscreenData.PlayerData[PlayerTwoID].OriginX = PlayerLayoutRatio;
	SplitscreenData.PlayerData[PlayerTwoID].SizeX = 1.f - PlayerLayoutRatio;
	
	SetTwoPlayerVerticalSplitScreenData(this, SplitscreenData);
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

FSplitscreenData UVSSplitScreenSubsystemW::GetTwoPlayerVerticalSplitScreenData(UObject* WorldContext)
{
	if (!WorldContext || !WorldContext->GetWorld()) return FSplitscreenData();
	FSplitscreenData SplitscreenData = VS_PRIVABLIC_MEMBER(WorldContext->GetWorld()->GetGameViewport(), UGameViewportClient, SplitscreenInfo)[ESplitScreenType::TwoPlayer_Vertical];
	return SplitscreenData;
}

void UVSSplitScreenSubsystemW::SetTwoPlayerVerticalSplitScreenData(UObject* WorldContext, const FSplitscreenData& Data)
{
	if (!WorldContext || !WorldContext->GetWorld()) return;
	FSplitscreenData& SplitscreenDataRef = VS_PRIVABLIC_MEMBER(WorldContext->GetWorld()->GetGameViewport(), UGameViewportClient, SplitscreenInfo)[ESplitScreenType::TwoPlayer_Vertical];
	SplitscreenDataRef = Data;
}