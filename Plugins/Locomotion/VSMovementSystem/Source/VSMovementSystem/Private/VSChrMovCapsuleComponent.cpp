// Copyright VanstySanic. All Rights Reserved.

#include "VSChrMovCapsuleComponent.h"

#include "VSPrivablic.h"
#include "GameFramework/Character.h"
#include "Libraries/VSGameplayLibrary.h"
#include "PhysicsEngine/SphylElem.h"
#include "PhysicsEngine/BodySetup.h"

VS_DECLARE_PRIVABLIC_MEMBER(USceneComponent, AttachChildren, TArray<TObjectPtr<USceneComponent>>);

UVSChrMovCapsuleComponent::UVSChrMovCapsuleComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSChrMovCapsuleComponent::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITORONLY_DATA
	/**
	 * If you directly set the character's capsule component class to this,
	 * the original capsule component object will still hold the former AttachChildren,
	 * and this will cause an ensure message and package failure.
	 * This will find the original root capsule component from the character and clear its AttachChildren.
	 */
	if (UVSGameplayLibrary::IsInBlueprintEditor(this))
	{
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			if (!GetAttachParent())
			{
				TArray<USceneComponent*> SceneComponents;
				Character->GetComponents<USceneComponent>(SceneComponents);
				for (USceneComponent* SceneComponent : SceneComponents)
				{
					if (SceneComponent == this) continue;
					bool bHasFoundOriginalCapsule = false;
					if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent->GetAttachParent()))
					{
						if (!CapsuleComponent->GetAttachParent())
						{
							for (TObjectIterator<UCapsuleComponent> It; It; ++It)
							{
								UCapsuleComponent* Instance = *It;
								if (Instance && Instance->GetAttachChildren().Contains(SceneComponent))
								{
									VS_PRIVABLIC_MEMBER(Instance, USceneComponent, AttachChildren).Empty();
									bHasFoundOriginalCapsule = true;
									break;
								}
							}
						}
					}
					if (bHasFoundOriginalCapsule) break;
				}
			}
		}
	}
#endif
}

FBoxSphereBounds UVSChrMovCapsuleComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FVector BoxPoint = FVector(CapsuleRadius,CapsuleRadius,CapsuleHalfHeight);
	return FBoxSphereBounds(FVector(0.0, 0.0, CapsuleCenterOffsetZ), BoxPoint, CapsuleHalfHeight).TransformBy(LocalToWorld);
}

FPrimitiveSceneProxy* UVSChrMovCapsuleComponent::CreateSceneProxy()
{
	/** Represents a UVSChrMovCapsuleComponent to the scene manager. */
	class FDrawCylinderSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawCylinderSceneProxy(const UVSChrMovCapsuleComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected( InComponent->bDrawOnlyIfSelected )
			, CapsuleRadius( InComponent->GetScaledCapsuleRadius() )
			, CapsuleHalfHeight( InComponent->GetScaledCapsuleHalfHeight() )
			, CapsuleCenterOffsetZ( InComponent->GetScaledCapsuleCenterOffsetZ() )
			, ShapeColor( InComponent->ShapeColor )
			, LineThickness( InComponent->LineThickness )
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);
			
			const FMatrix& LocalToWorld = GetLocalToWorld();
			const int32 CapsuleSides =  FMath::Clamp<int32>(CapsuleRadius / 4.f, 16, 64);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{

				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					const FLinearColor DrawCapsuleColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected() );

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					DrawWireCapsule( PDI, LocalToWorld.GetOrigin() + CapsuleCenterOffsetZ * LocalToWorld.GetUnitAxis(EAxis::Z), LocalToWorld.GetUnitAxis(EAxis::X), LocalToWorld.GetUnitAxis(EAxis::Y), LocalToWorld.GetUnitAxis(EAxis::Z), DrawCapsuleColor, CapsuleRadius, CapsuleHalfHeight, CapsuleSides, SDPG_World, LineThickness );
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint() const override { return( sizeof( *this ) + GetAllocatedSize() ); }
		uint32 GetAllocatedSize() const { return( FPrimitiveSceneProxy::GetAllocatedSize() ); }

	private:
		const uint32	bDrawOnlyIfSelected:1;
		const float		CapsuleRadius;
		const float		CapsuleHalfHeight;
		const float		CapsuleCenterOffsetZ;
		const FColor	ShapeColor;
		const float		LineThickness;
	};

	return new FDrawCylinderSceneProxy( this );
}

template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateCapsuleBodySetup(BodySetupType& ShapeBodySetup, bool bUseArchetypeBodySetup, float CapsuleRadius, float CapsuleHalfHeight, float CapsuleCenterOffsetZ)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(ShapeBodySetup->AggGeom.SphylElems.Num() == 1);
	FKSphylElem* SE = ShapeBodySetup->AggGeom.SphylElems.GetData();

	/** SphylElem uses height from center of capsule spheres, but UCapsuleComponent uses halfHeight from end of the sphere. */
	const float Length = 2 * FMath::Max(CapsuleHalfHeight - CapsuleRadius, 0.f);	

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		SE->SetTransform(FTransform::Identity);
		SE->Center = FVector(0.0, 0.0, CapsuleCenterOffsetZ);
		SE->Radius = CapsuleRadius;
		SE->Length = Length;
	}
	else
	{
		if(SE->Radius != CapsuleRadius || SE->Length != Length)
		{
			ShapeBodySetup = nullptr;
			bUseArchetypeBodySetup = false;
		}
	}
	
	return bUseArchetypeBodySetup;
}

void UVSChrMovCapsuleComponent::UpdateBodySetup()
{
	if (PrepareSharedBodySetup<UCapsuleComponent>())
	{
		bUseArchetypeBodySetup = InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, CapsuleRadius, CapsuleHalfHeight, CapsuleCenterOffsetZ);
	}

	CreateShapeBodySetupIfNeeded<FKSphylElem>();

	if (!bUseArchetypeBodySetup)
	{
		InvalidateOrUpdateCapsuleBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, CapsuleRadius, CapsuleHalfHeight, CapsuleCenterOffsetZ);
	}	
}

FVector UVSChrMovCapsuleComponent::GetCapsuleCenterLocation() const
{
	return GetComponentLocation() + GetScaledCapsuleCenterOffsetZ() * GetUpVector();
}

FVector UVSChrMovCapsuleComponent::GetCapsuleRootLocation() const
{
	return GetComponentLocation() + (GetScaledCapsuleCenterOffsetZ() - GetScaledCapsuleHalfHeight()) * GetUpVector();
}

void UVSChrMovCapsuleComponent::SetCapsuleHalfHeightAndKeepRoot(float NewHalfHeight, bool bUpdateOverlaps)
{
	const float PrevHalfHeight = CapsuleHalfHeight;
	const float HalfHeight = FMath::Max3(0.f, NewHalfHeight, CapsuleRadius);
	SetCapsuleHalfHeight(HalfHeight, bUpdateOverlaps);
	AddCapsuleCenterOffsetZ(HalfHeight - PrevHalfHeight);
}

void UVSChrMovCapsuleComponent::SetCapsuleCenterOffsetZ(float CenterOffsetZ, bool bUpdateOverlaps)
{
	CapsuleCenterOffsetZ = CenterOffsetZ;
	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();
	
	// do this if already created
	// otherwise, it hasn't been really created yet
	if (bPhysicsStateCreated)
	{
		// Update physics engine collision shapes
		BodyInstance.UpdateBodyScale(GetComponentTransform().GetScale3D(), true);
	
		if (bUpdateOverlaps && IsCollisionEnabled() && GetOwner())
		{
			UpdateOverlaps();
		}
	}
}

void UVSChrMovCapsuleComponent::AddCapsuleCenterOffsetZ(float DeltaCenterOffsetZ, bool bUpdateOverlaps)
{
	SetCapsuleCenterOffsetZ(CapsuleCenterOffsetZ + DeltaCenterOffsetZ, bUpdateOverlaps);
}

float UVSChrMovCapsuleComponent::GetScaledCapsuleCenterOffsetZ() const
{
	return CapsuleCenterOffsetZ * GetComponentScale().Z;
}
