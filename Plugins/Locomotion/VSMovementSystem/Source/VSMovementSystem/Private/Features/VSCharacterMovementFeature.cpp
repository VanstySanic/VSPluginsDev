// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementFeature.h"
#include "GameplayTagContainer.h"
#include "VSCharacterMovementUtils.h"
#include "VSChrMovCapsuleComponent.h"
#include "VSMovementSystemSettings.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/VSActorLibrary.h"

UVSCharacterMovementFeature::UVSCharacterMovementFeature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementFeature::Initialize_Implementation()
{
	Super::Initialize_Implementation();
	
	ChrMovFeatureAgentPrivate = Cast<UVSCharacterMovementFeatureAgent>(this);
	if (!ChrMovFeatureAgentPrivate.IsValid())
	{
		ChrMovFeatureAgentPrivate = UVSCharacterMovementUtils::GetCharacterMovementFeatureAgentFromActor(GetOwnerActor());
	}
	check(ChrMovFeatureAgentPrivate.IsValid());

	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementFeatureAgent::OnMovementTagEventNotified);
}

void UVSCharacterMovementFeature::Uninitialize_Implementation()
{
	if (UVSGameplayTagController* GameplayTagController = GetGameplayTagController())
	{
		GameplayTagController->OnTagsUpdated.RemoveDynamic(this, &UVSCharacterMovementFeature::OnMovementTagsUpdated);
		GameplayTagController->OnTagEventNotified.RemoveDynamic(this, &UVSCharacterMovementFeature::OnMovementTagEventNotified);
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

UVSChrMovCapsuleComponent* UVSCharacterMovementFeature::GetMovementCapsuleComponent() const
{
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->MovementCapsuleComponentPrivate.Get() : nullptr;
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

void UVSCharacterMovementFeature::SetMovementMode(const FGameplayTag& InMovementMode, bool bReplicated)
{
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated() && bReplicated)
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
	if (!GetCharacterMovement()) return FVector::ZeroVector;
	if (!UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && ChrMovFeatureAgentPrivate.IsValid())
	{
		return ChrMovFeatureAgentPrivate->ReplicatedMovementInput;
	}
	return GetCharacterMovement()->GetCurrentAcceleration();
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

FRotator UVSCharacterMovementFeature::GetControlRotation() const
{
	if (GetController()) { return GetController()->GetControlRotation().GetNormalized(); }
	return ChrMovFeatureAgentPrivate.IsValid() ? ChrMovFeatureAgentPrivate->ReplicatedControlRotation.GetNormalized() : FRotator::ZeroRotator;
}

FVector UVSCharacterMovementFeature::GetRootLocation() const
{
	return GetMovementCapsuleComponent() ? GetMovementCapsuleComponent()->GetCapsuleRootLocation() : FVector::ZeroVector;
}

FRotator UVSCharacterMovementFeature::EvaluateOrientation(const FVSMovementOrientationEvaluateParams& Params)
{
	if (!GetCharacter()) return FRotator::ZeroRotator;
	UVSCharacterMovementFeatureAgent* MovementFeatureAgent = GetMovementFeatureAgent();
	if (!MovementFeatureAgent) return GetCharacter()->GetActorRotation();

	const FVector& UpDirection = GetCharacter()->GetActorUpVector();
	const FQuat& WorldToUpDirectionRotation = FQuat::FindBetweenNormals(FVector::UpVector, UpDirection);
	const FQuat& UpDirectionToWorldRotation = WorldToUpDirectionRotation.Inverse();

	FRotator AnsRotationWS = GetCharacter()->GetActorRotation();

	switch (Params.EvaluateType.OrientationType)
	{
	case EVSMovementRelatedOrientationType::None:
	case EVSMovementRelatedOrientationType::Self:
		break;
		
	case EVSMovementRelatedOrientationType::Velocity:
		{
			const bool bOverrideVelocity = Params.OverridenRotationTypes.Contains(EVSMovementRelatedOrientationType::Velocity);
			const FVector& VelocityToProcess = !bOverrideVelocity ? GetVelocity() : Params.DynamicDataOverride.Velocity;
			FVector VelocityGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, VelocityToProcess);
			if (Params.bReturnRotationInSpace2D) VelocityGS.Z = 0.f;
			if (VelocityGS.IsNearlyZero(0.01f)) break;
			AnsRotationWS = UKismetMathLibrary::ComposeRotators(VelocityToProcess.Rotation(), UpDirectionToWorldRotation.Rotator());
		}
		break;
		
	case EVSMovementRelatedOrientationType::Input:
		{
			const bool bOverrideInput = Params.OverridenRotationTypes.Contains(EVSMovementRelatedOrientationType::Input);
			const FVector& InputToProcess = !bOverrideInput ? GetMovementInput() : Params.DynamicDataOverride.MovementInput;
			FVector InputGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, InputToProcess);
			if (Params.bReturnRotationInSpace2D) InputGS.Z = 0.f;
			if (InputGS.IsNearlyZero(0.01f)) break;
			AnsRotationWS = UKismetMathLibrary::ComposeRotators(InputGS.Rotation(), UpDirectionToWorldRotation.Rotator());
		}
		break;
		
	case EVSMovementRelatedOrientationType::Control:
		{
			const FRotator& TargetControl = !Params.OverridenRotationTypes.Contains(EVSMovementRelatedOrientationType::Custom) ? GetControlRotation() : Params.DynamicDataOverride.ControlRotation;
			AnsRotationWS = UKismetMathLibrary::ComposeRotators(TargetControl, UpDirectionToWorldRotation.Rotator());
		}
		break;
		
	case EVSMovementRelatedOrientationType::Custom:
		{
			AnsRotationWS = !Params.OverridenRotationTypes.Contains(EVSMovementRelatedOrientationType::Custom)
				? MovementFeatureAgent->CustomEvaluateRotation
				: Params.CustomRotation;
		}
		break;
		
	case EVSMovementRelatedOrientationType::Aim:
		{
			FVector AimDirectionWS = GetCharacter()->GetActorForwardVector();
			const bool bOverrideAimData = Params.OverridenRotationTypes.Contains(EVSMovementRelatedOrientationType::Aim);
			switch (Params.EvaluateType.AimTargetType)
			{
			case EVSMovementOrientationAimTargetType::None:
				break;
				
			case EVSMovementOrientationAimTargetType::Point:
				{
					const FVector& TargetPoint = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Point))
						? MovementFeatureAgent->OrientationAimData.Point
						: Params.AimDataOverride.Point;
					AimDirectionWS = TargetPoint - GetCharacter()->GetActorLocation();
				}
				break;
				
			case EVSMovementOrientationAimTargetType::Direction:
				{
					AimDirectionWS = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Direction))
						? MovementFeatureAgent->OrientationAimData.Direction
						: Params.AimDataOverride.Direction;
				}
				break;
				
			case EVSMovementOrientationAimTargetType::Actor:
				{
					AActor* Actor = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Actor))
						? MovementFeatureAgent->OrientationAimData.Actor.Get()
						: Params.AimDataOverride.Actor.Get();
					
					if (Actor)
					{
						AimDirectionWS = Actor->GetActorLocation() - GetCharacter()->GetActorLocation();
					}
				}
				break;
				
			case EVSMovementOrientationAimTargetType::Component:
				{
					USceneComponent* Component = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Component))
						? MovementFeatureAgent->OrientationAimData.Component.Get()
						: Params.AimDataOverride.Component.Get();
										
					if (Component)
					{
						AimDirectionWS = Component->GetComponentLocation() - GetCharacter()->GetActorLocation();
					}
				}
				break;
				
			case EVSMovementOrientationAimTargetType::Socket:
				{
					USceneComponent* Component = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Socket))
						? MovementFeatureAgent->OrientationAimData.SocketComponent.Get()
						: Params.AimDataOverride.SocketComponent.Get();
					FName SocketName = !(bOverrideAimData && Params.OverridenAimTargetTypes.Contains(EVSMovementOrientationAimTargetType::Socket))
						? MovementFeatureAgent->OrientationAimData.SocketName
						: Params.AimDataOverride.SocketName;
					
					if (Component)
					{
						AimDirectionWS = Component->GetSocketLocation(SocketName) - GetCharacter()->GetActorLocation();
					}
				}
				break;
				
			default: ;
			}

			FVector AimingDirectionGS = UKismetMathLibrary::Quat_RotateVector(UpDirectionToWorldRotation, AimDirectionWS);
			if (Params.bReturnRotationInSpace2D) AimingDirectionGS.Z = 0.f;
			AnsRotationWS = UKismetMathLibrary::ComposeRotators(AimingDirectionGS.Rotation(), WorldToUpDirectionRotation.Rotator());
		}
		break;
	
	default: ;
	}
	
	FRotator AnsRotationDS = UKismetMathLibrary::ComposeRotators(AnsRotationWS, UpDirectionToWorldRotation.Rotator());
	if (Params.bReturnRotationInSpace2D) AnsRotationDS.SetComponentForAxis(EAxis::Y, 0.f);
	AnsRotationDS.Normalize();
	AnsRotationWS = UKismetMathLibrary::ComposeRotators(AnsRotationDS, WorldToUpDirectionRotation.Rotator());
	return AnsRotationWS;
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