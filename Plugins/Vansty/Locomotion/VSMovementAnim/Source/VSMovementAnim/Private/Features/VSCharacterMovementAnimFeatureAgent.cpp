// Copyright VanstySanic. All Rights Reserved.

#include "Features/VSCharacterMovementAnimFeatureAgent.h"
#include "VSCharacterMovementUtils.h"
#include "VSMovementAnimUtils.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Features/VSCharacterMovementFeatureAgent.h"
#include "GameFramework/Character.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSGameplayLibrary.h"


UVSCharacterMovementAnimFeatureAgent::UVSCharacterMovementAnimFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSCharacterMovementAnimFeatureAgent::Initialize_Implementation()
{
	Super::Super::Initialize_Implementation();
	
	AnimFeatureAgentPrivate = this;

	CharacterPrivate = Cast<ACharacter>(UVSGameplayLibrary::GetPawnFromSubObject(this));
	check(CharacterPrivate.IsValid() && CharacterPrivate->Implements<UVSGameplayTagControllerInterface>());

	ChrMovAgentFeaturePrivate = UVSCharacterMovementUtils::GetCharacterMovementFeatureAgentFromActor(CharacterPrivate.Get());
	check(ChrMovAgentFeaturePrivate.IsValid());

	GameplayTagControllerPrivate = UVSActorLibrary::GetGameplayTagControllerFromActor(CharacterPrivate.Get());
	check(GameplayTagControllerPrivate.IsValid());

	GetGameplayTagController()->OnTagsUpdated.AddDynamic(this, &UVSCharacterMovementAnimFeatureAgent::OnMovementTagsUpdated);
	GetGameplayTagController()->OnTagEventNotified.AddDynamic(this, &UVSCharacterMovementAnimFeatureAgent::OnMovementTagEventNotified);
}

void UVSCharacterMovementAnimFeatureAgent::BeginPlay_Implementation()
{
	Super::BeginPlay_Implementation();
}

void UVSCharacterMovementAnimFeatureAgent::EndPlay_Implementation()
{
	Super::EndPlay_Implementation();
}

void UVSCharacterMovementAnimFeatureAgent::UpdateAnimation_Implementation(float DeltaTime)
{
	for (UVSCharacterMovementAnimFeature* SubFeature : FindSubFeaturesByClass<UVSCharacterMovementAnimFeature>())
	{
		if (SubFeature)
		{
			SubFeature->UpdateAnimation(DeltaTime);
		}
	}
}

void UVSCharacterMovementAnimFeatureAgent::UpdateAnimationThreadSafe_Implementation(float DeltaTime)
{
	AnimData.bMovementModeChangedThisFrame = AnimData.CachedMovementMode != GetMovementMode();
	AnimData.CachedMovementMode = GetMovementMode();
	AnimData.AnimVelocityDirectionToCharacter2D = UVSMovementAnimUtils::CalcAnimDirectionByTwoVector2D(GetAnimVelocity2D(), GetCharacter()->GetActorForwardVector(), GetUpDirection(), DirectionBufferAngle, AnimData.AnimVelocityDirectionToCharacter2D);
	
	for (UVSCharacterMovementAnimFeature* SubFeature : FindSubFeaturesByClass<UVSCharacterMovementAnimFeature>())
	{
		if (SubFeature)
		{
			SubFeature->UpdateAnimationThreadSafe(DeltaTime);
		}
	}
}

void UVSCharacterMovementAnimFeatureAgent::OnMovementTagsUpdated_Implementation()
{
	Super::OnMovementTagsUpdated_Implementation();
}

void UVSCharacterMovementAnimFeatureAgent::OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent)
{
	Super::OnMovementTagEventNotified_Implementation(TagEvent);
}
