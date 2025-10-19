// Copyright VanstySanic. All Rights Reserved.

#include "Interactive/Feature/VSInteractiveOutlineFeature.h"

#include "Classes/Queries/VSSceneComponentQueryExpression.h"
#include "Components/BrushComponent.h"
#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"

UVSInteractiveOutlineFeature::UVSInteractiveOutlineFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponentQuery.RootExpression = CreateDefaultSubobject<UVSSceneComponentQueryExpression>(TEXT("RootExpression"));
	MeshComponentQuery.RootExpression->Type = EVSQueryMatchType::Params;
	MeshComponentQuery.RootExpression->Range = EVSQueryMatchRange::Any;
	FVSSceneComponentQueryParams Params;
	Params.ComponentClasses.Add(UStaticMeshComponent::StaticClass());
	Params.ComponentClasses.Add(USkeletalMeshComponent::StaticClass());
	MeshComponentQuery.RootExpression->Params.Add(Params);
}

void UVSInteractiveOutlineFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	GetInteractiveFeatureAgent()->OnInspectionStart.AddDynamic(this, &UVSInteractiveOutlineFeature::OnInspectionStart);
	GetInteractiveFeatureAgent()->OnInspectionEnd.AddDynamic(this, &UVSInteractiveOutlineFeature::OnInspectionEnd);
	// GetInteractiveFeatureAgent()->OnInteractionStart.AddDynamic(this, &UVSInteractiveOutlineFeature::OnInteractionStart);
}

void UVSInteractiveOutlineFeature::Uninitialize_Implementation()
{
	if (UVSInteractiveFeatureAgent* Agent = GetInteractiveFeatureAgent())
	{
		Agent->OnInspectionStart.RemoveDynamic(this, &UVSInteractiveOutlineFeature::OnInspectionStart);
		Agent->OnInspectionStart.RemoveDynamic(this, &UVSInteractiveOutlineFeature::OnInspectionEnd);
		// Agent->OnInteractionStart.RemoveDynamic(this, &UVSInteractiveOutlineFeature::OnInteractionStart);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSInteractiveOutlineFeature::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
	
	TArray<UMeshComponent*> MeshComponents;
	GetOwnerActor()->GetComponents<UMeshComponent>(MeshComponents);
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		if (!MeshComponentQuery.Matches(MeshComponent)) continue;
		MeshComponent->SetRenderCustomDepth(false);
	}
}

void UVSInteractiveOutlineFeature::OnFeatureDeactivated_Implementation()
{
	SetShouldShowOutline(false);
	HideOutlineImmediately();
	
	Super::OnFeatureDeactivated_Implementation();
}

void UVSInteractiveOutlineFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
	
	// const bool bSkipBlend = (bShouldShowOutline && FMath::IsNearlyEqual(OutlineWeightBlendTime, WeightAlphaBlendArgs.BlendTime)) || (!bShouldShowOutline && FMath::IsNearlyZero(OutlineWeightBlendTime));
	// if (!bSkipBlend && (bOutlineWeightBlendIn && bShouldShowOutline) || (bOutlineWeightBlendOut && !bShouldShowOutline))
	// {
	// 	const float DirectionSign = bShouldShowOutline ? 1.f : -1.f;
	// 	if (!(DirectionSign > 0.f && FMath::IsNearlyEqual(OutlineWeightBlendTime, WeightAlphaBlendArgs.BlendTime)) && !(DirectionSign < 0.f && FMath::IsNearlyZero(OutlineWeightBlendTime)))
	// 	{
	// 		OutlineWeightBlendTime = FMath::Clamp(OutlineWeightBlendTime + DirectionSign * DeltaTime, 0.f, WeightAlphaBlendArgs.BlendTime);
	// 		const float LinerAlpha = UKismetMathLibrary::MapRangeClamped(OutlineWeightBlendTime, 0.f, WeightAlphaBlendArgs.BlendTime, 0.f, 1.f);
	// 		const float OpacityAlpha = FAlphaBlend::AlphaToBlendOption(LinerAlpha, WeightAlphaBlendArgs.BlendOption, WeightAlphaBlendArgs.CustomCurve);
	//
	// 		/** Update weights. */
	// 		for (auto& MeshDataMap : OutlineMeshDataMap)
	// 		{
	// 			int32 MaterialNum = MeshDataMap.Value.PostProcessMaterials.Array.Num();
	// 			for (int i = 0;i < MaterialNum;i++)
	// 			{
	// 				const float MaxWeight = MeshDataMap.Value.PostProcessMaterials.Array[i].Weight;
	// 				float Weight = UKismetMathLibrary::MapRangeClamped(OpacityAlpha, 0.f, 1.f, 0.f, MaxWeight);
	// 				MeshDataMap.Value.PostProcessVolume->Settings.WeightedBlendables.Array[i].Weight = Weight;
	// 			}
	// 		}
	// 		
	// 		if (!bShouldShowOutline && FMath::IsNearlyZero(OutlineWeightBlendTime))
	// 		{
	// 			HideOutlineImmediately();
	// 		}
	// 	}
	// }
}

void UVSInteractiveOutlineFeature::SetShouldShowOutline(bool bShouldShow)
{
	if (bShouldShowOutline == bShouldShow) return;
	bShouldShowOutline = bShouldShow;
	if (bShouldShowOutline)
	{
		TArray<UMeshComponent*> MeshComponents;
		GetOwnerActor()->GetComponents<UMeshComponent>(MeshComponents);
		for (UMeshComponent* MeshComponent : MeshComponents)
		{
			if (!MeshComponentQuery.Matches(MeshComponent)) continue;

			FOutlineMeshData& OutlineMeshData = OutlineMeshDataMap.FindOrAdd(MeshComponent);
			
			MeshComponent->SetRenderCustomDepth(true);
			if (bOverrideCustomDepthStencilValue)
			{
				OutlineMeshData.OriginalCustomDepthStencilValue = MeshComponent->CustomDepthStencilValue;
				MeshComponent->SetCustomDepthStencilValue(CustomDepthStencilValue);
			}

			// /** Spawn post process actor for mesh if not exist. */
			// if (!OutlineMeshData.PostProcessVolume)
			// {
			// 	FActorSpawnParameters Params;
			// 	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			// 	APostProcessVolume* MeshPostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), MeshComponent->GetComponentTransform(), Params);
			// 	check(MeshPostProcessVolume);
			//
			// 	MeshPostProcessVolume->GetBrushComponent()->Bounds.BoxExtent = MeshComponent->Bounds.BoxExtent * 1.01f;
			// 	MeshPostProcessVolume->AttachToComponent(MeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
			// 	MeshPostProcessVolume->SetActorRelativeTransform(FTransform::Identity);
			// 	
			// 	MeshPostProcessVolume->bUnbound = false;
			// 	MeshPostProcessVolume->Settings.WeightedBlendables = bOverridePostProcessMaterials ? PostProcessMaterials : MeshPostProcessVolume->Settings.WeightedBlendables;
			// 	OutlineMeshData.PostProcessVolume = MeshPostProcessVolume;
			// 	OutlineMeshData.PostProcessMaterials = MeshPostProcessVolume->Settings.WeightedBlendables;
			// }
		}
		// if (!bOutlineWeightBlendOut)
		// {
		// 	ShowOutlineImmediately();
		// }
	}
	else if (!bShouldShowOutline/* && !bOutlineWeightBlendOut*/)
	{
		HideOutlineImmediately();
	}
}

void UVSInteractiveOutlineFeature::ShowOutlineImmediately()
{
	for (auto& MeshDataMap : OutlineMeshDataMap)
	{
		if (MeshDataMap.Key.IsValid())
		{
			MeshDataMap.Key->SetRenderCustomDepth(true);
			if (bOverrideCustomDepthStencilValue)
			{
				MeshDataMap.Value.OriginalCustomDepthStencilValue = MeshDataMap.Key->CustomDepthStencilValue;
				MeshDataMap.Key->SetCustomDepthStencilValue(CustomDepthStencilValue);
			}
		}
		
		// const int32 MaterialNum = MeshDataMap.Value.PostProcessMaterials.Array.Num();
		// for (int i = 0;i < MaterialNum;i++)
		// {
		// 	MeshDataMap.Value.PostProcessVolume->Settings.WeightedBlendables.Array[i].Weight = MeshDataMap.Value.PostProcessMaterials.Array[i].Weight;
		// }
	}
	// OutlineWeightBlendTime = WeightAlphaBlendArgs.BlendTime;
}

void UVSInteractiveOutlineFeature::HideOutlineImmediately()
{
	for (auto& MeshDataMap : OutlineMeshDataMap)
	{
		if (bOverrideCustomDepthStencilValue)
		{
			if (MeshDataMap.Key.IsValid())
			{
				MeshDataMap.Key->SetCustomDepthStencilValue(MeshDataMap.Value.OriginalCustomDepthStencilValue);
			}
		}
		// if (MeshDataMap.Value.PostProcessVolume)
		// {
		// 	MeshDataMap.Value.PostProcessVolume->Destroy();
		// }
	}
	OutlineMeshDataMap.Empty();
	// OutlineWeightBlendTime = 0.f;
}

void UVSInteractiveOutlineFeature::OnInspectionStart(UVSInteractFeatureAgent* SourceAgent)
{
	if (SourceAgent && UVSActorLibrary::IsActorNetLocal(SourceAgent->GetOwnerActor()))
	{
		SetShouldShowOutline(true);
	}
}

void UVSInteractiveOutlineFeature::OnInspectionEnd(UVSInteractFeatureAgent* SourceAgent)
{
	if (SourceAgent && UVSActorLibrary::IsActorNetLocal(SourceAgent->GetOwnerActor()))
	{
		// if (!SourceAgent->IsInteracting() || SourceAgent->GetCurrentInteractiveFeatureAgent() != GetInteractiveFeatureAgent())
		{
			SetShouldShowOutline(false);
		}
	}
}

// void UVSInteractiveOutlineFeature::OnInteractionStart(UVSInteractFeatureAgent* SourceAgent, FName ActionFeatureName)
// {
// 	if (SourceAgent && UVSActorLibrary::IsActorLocal(SourceAgent->GetOwnerActor()))
// 	{
// 		SetShouldShowOutline(false);
// 		// if (bOutlineHideImmediatelyWithInteraction)
// 		// {
// 		// 	HideOutlineImmediately();
// 		// }
// 	}
// }
