// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSMovementFeatureBase.h"
#include "Classes/Features/VSGameplayTagFeature.h"
#include "Classes/Libraries/VSObjectLibrary.h"

UVSMovementFeatureBase::UVSMovementFeatureBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSMovementFeatureBase::TickFeature(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction)
{
	Super::TickFeature(DeltaTime, TickType, TickFunction);

	if (CanUpdateMovement() && !FMath::IsNearlyZero(DeltaTime))
	{
		UpdateMovement(DeltaTime);
	}
}

UVSGameplayTagFeatureBase* UVSMovementFeatureBase::GetPrimaryGameplayTagFeature_Implementation() const
{
	if (!GetOwnerActor()) return nullptr;
	if (GetOwnerActor()->GetClass()->ImplementsInterface(UVSGameplayTagFeatureInterface::StaticClass()))
	{
		return Execute_GetPrimaryGameplayTagFeature(GetOwnerActor());
	}

	return UVSObjectLibrary::FindFeatureByClassFromObject<UVSGameplayTagFeatureBase>(GetOwnerActor());
}

void UVSMovementFeatureBase::OnMovementModeChanged_Implementation(const FGameplayTag& NewMovementMode, const FGameplayTag& PrevMovementMode)
{
	
}

void UVSMovementFeatureBase::UpdateMovement_Implementation(float DeltaTime)
{
	
}

bool UVSMovementFeatureBase::CanUpdateMovement_Implementation() const
{
	return IsActive() && GetUpdatedComponent();
}

FName UVSMovementFeatureBase::GetMovementBaseBoneName_Implementation() const
{
	return NAME_None;
}

FVector UVSMovementFeatureBase::GetVelocity_Implementation() const
{
	return GetOwnerActor() ? GetOwnerActor()->GetVelocity() : FVector::ZeroVector;
}

FVector UVSMovementFeatureBase::GetAngularVelocity_Implementation() const
{
	return FVector::ZeroVector;
}

FVector UVSMovementFeatureBase::GetAcceleration_Implementation() const
{
	return FVector::ZeroVector;
}

FVector UVSMovementFeatureBase::GetMovementInput_Implementation() const
{
	return FVector::ZeroVector;
}

USceneComponent* UVSMovementFeatureBase::GetUpdatedComponent_Implementation() const
{
	return GetOwnerActor() ? GetOwnerActor()->GetRootComponent() : nullptr;
}

UPrimitiveComponent* UVSMovementFeatureBase::GetMovementBase_Implementation() const
{
	return nullptr;
}

FGameplayTag UVSMovementFeatureBase::GetMovementMode_Implementation() const
{
	return FGameplayTag::EmptyTag;
}
