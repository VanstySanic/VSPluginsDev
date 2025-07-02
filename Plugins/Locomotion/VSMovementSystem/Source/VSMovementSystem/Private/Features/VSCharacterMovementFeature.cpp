// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeature.h"
#include "GameplayTagContainer.h"
#include "VSMovementSystemSettings.h"
#include "Classees/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Libraries/VSActorLibrary.h"

UVSCharacterMovementFeature::UVSCharacterMovementFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	if (IsA<UVSCharacterMovementFeatureAgent>()) { ChrMovFeatureAgentPrivate = Cast<UVSCharacterMovementFeatureAgent>(this); }
	else { ChrMovFeatureAgentPrivate = GetTypedOuter<UVSCharacterMovementFeatureAgent>(); }
	check(ChrMovFeatureAgentPrivate.IsValid());

	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagEventNotified);
}

void UVSCharacterMovementFeature::Uninitialize_Implementation()
{
	if (GetGameplayTagController())
	{
		GetGameplayTagController()->OnTagsUpdated.RemoveDynamic(this, &UVSCharacterMovementFeature::OnMovementTagsUpdated);
		GetGameplayTagController()->OnTagEventNotified.RemoveDynamic(this, &UVSCharacterMovementFeature::OnMovementTagEventNotified);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSCharacterMovementFeature::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);
	
	if (CanUpdateMovement())
	{
		UpdateMovement(DeltaTime);
	}
}

UVSCharacterMovementFeatureAgent* UVSCharacterMovementFeature::GetMovementAgentFeature_Implementation() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate.Get() : nullptr;
}

UCharacterMovementComponent* UVSCharacterMovementFeature::GetCharacterMovement() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->CharacterMovementComponentPrivate.Get() : nullptr;
}

ACharacter* UVSCharacterMovementFeature::GetCharacter() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->CharacterPrivate.Get() : nullptr;
}

AController* UVSCharacterMovementFeature::GetController() const
{
	return GetCharacter() ? GetCharacter()->Controller.Get() : nullptr;
}

UVSGameplayTagController* UVSCharacterMovementFeature::GetGameplayTagController() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->GameplayTagControllerPrivate.Get() : nullptr;
}

FGameplayTag UVSCharacterMovementFeature::GetMovementMode() const
{
	if (!GetCharacterMovement()) return FGameplayTag::EmptyTag;
	return UVSMovementSystemSettings::GetTagFromMovementMode(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode);
}

FGameplayTag UVSCharacterMovementFeature::GetPrevMovementMode() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->MovementData.PrevMovementMode : FGameplayTag::EmptyTag;
}

void UVSCharacterMovementFeature::SetMovementMode(const FGameplayTag& InMovementMode)
{
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()))
	{
		SetMovementMode_Server(InMovementMode);
	}
	else
	{
		SetMovementModeInternal(InMovementMode);
	}
}

FVector UVSCharacterMovementFeature::GetVelocity() const
{
	return GetCharacter() ? GetCharacter()->GetVelocity() : FVector::ZeroVector;
}

FVector UVSCharacterMovementFeature::GetVelocity2D() const
{
	return FVector::VectorPlaneProject(GetVelocity(), GetUpDirection());
}

FVector UVSCharacterMovementFeature::GetVelocityWallAdjusted2D() const
{
	if (!IsMovingAgainstWall2D()) return GetVelocity2D();
	const FVector& DesiredMovementDirection = (HasMovementInput2D() ? GetMovementInput2D() : GetVelocity2D()).GetSafeNormal();
	const float VelInputDot = GetVelocity2D().GetSafeNormal().Dot(GetMovementInput2D().GetSafeNormal());
	return DesiredMovementDirection * GetVelocity2D().Size() * VelInputDot;
}

FVector UVSCharacterMovementFeature::GetVelocityZ() const
{
	return GetVelocity().ProjectOnToNormal(-GetUpDirection());
}

float UVSCharacterMovementFeature::GetSpeed2D() const
{
	return GetVelocity2D().Size();
}

float UVSCharacterMovementFeature::GetSpeedWallAdjusted2D() const
{
	return GetVelocityWallAdjusted2D().Size();
}

float UVSCharacterMovementFeature::GetSpeedZ() const
{
	return GetVelocityZ().Size();
}

FVector UVSCharacterMovementFeature::GetMovementInput() const
{
	return GetCharacterMovement() ? GetCharacterMovement()->GetCurrentAcceleration() : FVector::ZeroVector;
}

FVector UVSCharacterMovementFeature::GetMovementInput2D() const
{
	return FVector::VectorPlaneProject(GetMovementInput(), GetUpDirection());
}

bool UVSCharacterMovementFeature::HasMovementInput() const
{
	return !GetMovementInput().IsNearlyZero(1.f);
}

bool UVSCharacterMovementFeature::HasMovementInput2D() const
{
	return !GetMovementInput2D().IsNearlyZero(1.f);
}

FVector UVSCharacterMovementFeature::GetScale3D() const
{
	return GetCharacter() ? GetCharacter()->GetActorScale3D() : FVector(1.0);
}

FVector UVSCharacterMovementFeature::GetRealAcceleration() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->MovementData.RealAcceleration : FVector::ZeroVector;
}

FVector UVSCharacterMovementFeature::GetRealAcceleration2D() const
{
	return FVector::VectorPlaneProject(GetRealAcceleration(), GetUpDirection());
}


bool UVSCharacterMovementFeature::IsMoving2D() const
{
	return !GetVelocity2D().IsNearlyZero(1.f)/* || HasMovementInput2D()*/;
}

bool UVSCharacterMovementFeature::IsMovingAgainstWall2D() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->MovementData.bIsMovingAgainstWall2D : false;
}

FVector UVSCharacterMovementFeature::GetUpDirection() const
{
	return GetCharacter() ? GetCharacter()->GetActorUpVector() : FVector::UpVector;
}

FVector UVSCharacterMovementFeature::GetGravityDirection() const
{
	return GetCharacter() ? GetCharacter()->GetGravityDirection() : FVector::DownVector;
}

void UVSCharacterMovementFeature::UpdateMovement_Implementation(float DeltaTime)
{
	
}

bool UVSCharacterMovementFeature::CanUpdateMovement_Implementation() const
{
	return IsActive();
}

void UVSCharacterMovementFeature::OnMovementTagsUpdated_Implementation()
{
	
}

void UVSCharacterMovementFeature::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	
}

void UVSCharacterMovementFeature::SetMovementMode_Server_Implementation(const FGameplayTag& InMovementMode)
{
	SetMovementModeInternal(InMovementMode);
}

void UVSCharacterMovementFeature::SetMovementModeInternal(const FGameplayTag& InMovementMode)
{
	if (!GetCharacterMovement()) return;
	TEnumAsByte<EMovementMode> OutMovementMode;
	uint8 OutCustomMode;
	UVSMovementSystemSettings::GetMovementModeFromTag(InMovementMode, OutMovementMode, OutCustomMode);
	GetCharacterMovement()->SetMovementMode(OutMovementMode, OutCustomMode);
}