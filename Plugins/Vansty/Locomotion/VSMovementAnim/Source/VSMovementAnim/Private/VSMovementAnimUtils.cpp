// Copyright VanstySanic. All Rights Reserved.

#include "VSMovementAnimUtils.h"
#include "VSCharacterMovementAnimInterface.h"
#include "Features/VSCharacterMovementAnimFeature.h"
#include "Features/VSCharacterMovementAnimFeatureAgent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"
#include "Types/VSMovementAnimTypes.h"
#include "Types/VSMovementAnimTags.h"

UVSMovementAnimUtils::UVSMovementAnimUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UVSCharacterMovementAnimFeatureAgent* UVSMovementAnimUtils::GetCharacterMovementAnimFeatureAgentFromActor(AActor* Actor)
{
	if (!Actor) return nullptr;
	UVSCharacterMovementAnimFeatureAgent* FeatureAgent = nullptr;
	if (!FeatureAgent)
	{
		if (Actor->GetClass()->ImplementsInterface(UVSCharacterMovementAnimInterface::StaticClass()))
		{
			FeatureAgent = IVSCharacterMovementAnimInterface::Execute_GetCharacterMovementAnimFeatureAgent(Actor);
		}
	}
	if (!FeatureAgent)
	{
		FeatureAgent = UVSActorLibrary::FindFeatureByClassFromActor<UVSCharacterMovementAnimFeatureAgent>(Actor);
	}
	return FeatureAgent;
}

FGameplayTag UVSMovementAnimUtils::CalcAnimDirectionByAngle2D(const float Angle, const float BufferAngle, const FGameplayTag& PrevDirection)
{
	static TArray<FGameplayTag> DirectionOrder = TArray<FGameplayTag>
	{
		EVSAnimDirection::BL, EVSAnimDirection::XL,
		EVSAnimDirection::FL, EVSAnimDirection::FX,
		EVSAnimDirection::FR, EVSAnimDirection::XR,
		EVSAnimDirection::BR, EVSAnimDirection::BX,
	};
	static float RangeAngle = 45.f;
	static float HalfRangeAngle = 22.5f;

	int32 PrevDirectionIndex;
	DirectionOrder.Find(PrevDirection, PrevDirectionIndex);

	FVector2D Buffer;
	Buffer.X = PrevDirectionIndex == 7 ? BufferAngle : 0.f;
	Buffer.Y = PrevDirectionIndex == 7 ? -BufferAngle : 0.f;
	Buffer.X = PrevDirectionIndex == 0 ? -BufferAngle : Buffer.X;
	Buffer.Y = PrevDirectionIndex == 6 ? BufferAngle : Buffer.Y;
	if (!UKismetMathLibrary::InRange_FloatFloat(Angle, -180.f + HalfRangeAngle + Buffer.X, 180.f - HalfRangeAngle + Buffer.Y))
	{
		return EVSAnimDirection::BX;
	}
	
	for (int i = 0;i <= 6;i++)
	{
		const float RangeMin = (i - 3) * RangeAngle - HalfRangeAngle;
		const float RangeMax = RangeMin + RangeAngle;
		if (PrevDirectionIndex == (i + 7) % 8)
		{
			Buffer.X = BufferAngle;
			Buffer.Y = 0.f;
		}
		else if (PrevDirectionIndex == i + 1)
		{
			Buffer.X = 0.f;
			Buffer.Y = -BufferAngle;
		}
		else if (PrevDirectionIndex == i)
		{
			Buffer.X = -BufferAngle;
			Buffer.Y = BufferAngle;
		}
		else
		{
			Buffer = FVector2D::ZeroVector;
		}
		
		if (UKismetMathLibrary::InRange_FloatFloat(Angle, RangeMin + Buffer.X, RangeMax + Buffer.Y))
		{
			return DirectionOrder[i];
		}
	}
	
	return EVSAnimDirection::XX;
}

FGameplayTag UVSMovementAnimUtils::CalcAnimDirectionByTwoVector2D(const FVector& Current, const FVector& RelativeTo, const FVector& UpDirection, const float BufferAngle, const FGameplayTag& PrevDirection)
{
	if (Current.IsNearlyZero(0.01f)) { return  EVSAnimDirection::XX; }
	const FVector& Current2DDS = FVector::VectorPlaneProject(Current, UpDirection);
	const FVector& RelativeTo2DDS = FVector::VectorPlaneProject(RelativeTo, UpDirection);
	const FQuat& DeltaRotation2DRS = FQuat::FindBetweenVectors(RelativeTo2DDS, Current2DDS);
	const float Sign = FMath::Sign(RelativeTo.Cross(Current2DDS).Dot(UpDirection));
	return CalcAnimDirectionByAngle2D(Sign * FMath::RadiansToDegrees(DeltaRotation2DRS.GetAngle()), BufferAngle, PrevDirection);
}

UAnimSequence* UVSMovementAnimUtils::GetAnimSequenceFromKeyedMapContainer(const FVSKeyedAnimSequenceMapContainer& KeyedAnimSequenceMaps, const FVSGameplayTagKey& GameplayTagKey)
{
	return KeyedAnimSequenceMaps.GetAnimSequence(GameplayTagKey);
}

void UVSMovementAnimUtils::UpdateAnimFeaturesThreadSafe(const TArray<UVSCharacterMovementAnimFeature*>& AnimFeatures, float DeltaTime)
{
	for (UVSCharacterMovementAnimFeature* AnimFeature : AnimFeatures)
	{
		if (AnimFeature)
		{
			AnimFeature->UpdateAnimationThreadSafe(DeltaTime);
		}
	}
}
