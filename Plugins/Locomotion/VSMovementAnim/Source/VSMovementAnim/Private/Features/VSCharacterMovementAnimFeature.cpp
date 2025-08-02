// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementAnimFeature.h"

#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementAnimFeatureAgent.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Types/VSMovementAnimTags.h"

UVSCharacterMovementAnimFeature::UVSCharacterMovementAnimFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryObjectTick.bStartWithTickEnabled = false;
	PrimaryObjectTick.TickGroup = TG_DuringPhysics;
	PrimaryObjectTick.EndTickGroup = TG_DuringPhysics;
}

void UVSCharacterMovementAnimFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	if (IsA<UVSCharacterMovementAnimFeatureAgent>()) { AnimFeatureAgentPrivate = Cast<UVSCharacterMovementAnimFeatureAgent>(this); }
	else { AnimFeatureAgentPrivate = GetTypedOuter<UVSCharacterMovementAnimFeatureAgent>(); }
	check(AnimFeatureAgentPrivate.IsValid());

	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementAnimFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementAnimFeatureAgent::OnMovementTagEventNotified);
}

ACharacter* UVSCharacterMovementAnimFeature::GetCharacter() const
{
	return AnimFeatureAgentPrivate.IsValid() ? AnimFeatureAgentPrivate->CharacterPrivate.Get() : nullptr;
}

UCharacterMovementComponent* UVSCharacterMovementAnimFeature::GetCharacterMovement() const
{
	return GetCharacter() ? GetCharacter()->GetCharacterMovement() : nullptr;
}

AController* UVSCharacterMovementAnimFeature::GetController() const
{
	return GetCharacter() ? GetCharacter()->GetController() : nullptr;
}

UVSCharacterMovementFeatureAgent* UVSCharacterMovementAnimFeature::GetMovementFeatureAgent() const
{
	return AnimFeatureAgentPrivate.IsValid() ? AnimFeatureAgentPrivate->ChrMovAgentFeaturePrivate.Get() : nullptr;
}

UVSGameplayTagController* UVSCharacterMovementAnimFeature::GetGameplayTagController() const
{
	return AnimFeatureAgentPrivate.IsValid() ? AnimFeatureAgentPrivate->GameplayTagControllerPrivate.Get() : nullptr;
}

FGameplayTag UVSCharacterMovementAnimFeature::GetMovementMode() const
{
	if (!GetMovementFeatureAgent()) return FGameplayTag::EmptyTag;
	return GetMovementFeatureAgent()->GetMovementMode();
}

FGameplayTag UVSCharacterMovementAnimFeature::GetPrevMovementMode() const
{
	return GetMovementFeatureAgent() ? GetMovementFeatureAgent()->GetPrevMovementMode() : FGameplayTag::EmptyTag;
}

FVector UVSCharacterMovementAnimFeature::GetVelocity() const
{
	return GetCharacter() ? GetCharacter()->GetVelocity() : FVector::ZeroVector;
}

FVector UVSCharacterMovementAnimFeature::GetVelocity2D() const
{
	return FVector::VectorPlaneProject(GetVelocity(), GetUpDirection());
}

FVector UVSCharacterMovementAnimFeature::GetAnimVelocity2D() const
{
	if (!IsMovingAgainstWall2D()) return GetVelocity2D();
	const FVector& DesiredMovementDirection = (HasMovementInput2D() ? GetMovementInput2D() : GetVelocity2D()).GetSafeNormal();
	const float VelInputDot = GetVelocity2D().GetSafeNormal().Dot(GetMovementInput2D().GetSafeNormal());
	return DesiredMovementDirection * GetVelocity2D().Size() * VelInputDot;
}

FVector UVSCharacterMovementAnimFeature::GetVelocityZ() const
{
	return GetVelocity().ProjectOnToNormal(-GetUpDirection());
}

float UVSCharacterMovementAnimFeature::GetSpeed2D() const
{
	return GetVelocity2D().Size();
}

float UVSCharacterMovementAnimFeature::GetAnimSpeed2D() const
{
	return GetAnimVelocity2D().Size();
}

float UVSCharacterMovementAnimFeature::GetSpeedZ() const
{
	return GetVelocityZ().Size();
}

FVector UVSCharacterMovementAnimFeature::GetMovementInput() const
{
	return GetMovementFeatureAgent() ? GetMovementFeatureAgent()->GetMovementInput() : FVector::ZeroVector;
}

FVector UVSCharacterMovementAnimFeature::GetMovementInput2D() const
{
	return FVector::VectorPlaneProject(GetMovementInput(), GetUpDirection());
}

bool UVSCharacterMovementAnimFeature::HasMovementInput() const
{
	return !GetMovementInput().IsNearlyZero(1.f);
}

bool UVSCharacterMovementAnimFeature::HasMovementInput2D() const
{
	return !GetMovementInput2D().IsNearlyZero(1.f);
}

FVector UVSCharacterMovementAnimFeature::GetScale3D() const
{
	return GetCharacter() ? GetCharacter()->GetActorScale3D() : FVector(1.0);
}

FVector UVSCharacterMovementAnimFeature::GetRealAcceleration() const
{
	return GetMovementFeatureAgent() ? GetMovementFeatureAgent()->GetRealAcceleration() : FVector::ZeroVector;
}

FVector UVSCharacterMovementAnimFeature::GetRealAcceleration2D() const
{
	return FVector::VectorPlaneProject(GetRealAcceleration(), GetUpDirection());
}

bool UVSCharacterMovementAnimFeature::IsMoving2D() const
{
	return !GetVelocity2D().IsNearlyZero(1.f)/* || HasMovementInput2D()*/;
}

bool UVSCharacterMovementAnimFeature::IsMovingAgainstWall2D() const
{
	return GetMovementFeatureAgent() ? GetMovementFeatureAgent()->IsMovingAgainstWall2D() : false;
}

FVector UVSCharacterMovementAnimFeature::GetUpDirection() const
{
	return GetCharacter() ? GetCharacter()->GetActorUpVector() : FVector::UpVector;
}

FVector UVSCharacterMovementAnimFeature::GetGravityDirection() const
{
	return GetCharacterMovement() ? GetCharacterMovement()->GetGravityDirection() : FVector::DownVector;
}

FRotator UVSCharacterMovementAnimFeature::GetControlRotation() const
{
	return GetMovementFeatureAgent() ? GetMovementFeatureAgent()->GetControlRotation() : FRotator::ZeroRotator;
}

FGameplayTag UVSCharacterMovementAnimFeature::GetAnimVelocityDirectionToCharacter2D() const
{
	return AnimFeatureAgentPrivate.IsValid() ? AnimFeatureAgentPrivate->AnimData.AnimVelocityDirectionToCharacter2D : EVSAnimDirection::XX;
}

void UVSCharacterMovementAnimFeature::UpdateAnimation_Implementation(float DeltaTime)
{
}

void UVSCharacterMovementAnimFeature::UpdateAnimationThreadSafe_Implementation(float DeltaTime)
{
}

void UVSCharacterMovementAnimFeature::OnMovementTagsUpdated_Implementation()
{
}

void UVSCharacterMovementAnimFeature::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
}