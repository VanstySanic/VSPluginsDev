// Copyright VanstySanic. All Rights Reserved.

#include "Classees/Framework/VSGameplayTagController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSPrivablicLibrary.h"

VS_DECLARE_PRIVABLIC_MEMBER(UAbilitySystemComponent, GameplayTagCountContainer, FGameplayTagCountContainer);
VS_DECLARE_PRIVABLIC_MEMBER(FGameplayTagCountContainer, OnAnyTagChangeDelegate, FOnGameplayEffectTagCountChanged);

UVSGameplayTagController::UVSGameplayTagController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSGameplayTagController::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	AbilitySystemComponentPrivate = GetAbilitySystemComponent();
	check(AbilitySystemComponentPrivate.IsValid());
	FGameplayTagCountContainer& GameplayTagCountContainer = VS_PRIVABLIC_MEMBER(AbilitySystemComponentPrivate.Get(), UAbilitySystemComponent, GameplayTagCountContainer);
	OnAnyTagChangeDelegateHandle = VS_PRIVABLIC_MEMBER(&GameplayTagCountContainer, FGameplayTagCountContainer, OnAnyTagChangeDelegate).AddUObject(this, &UVSGameplayTagController::OnAnyTagChange);
}

void UVSGameplayTagController::Uninitialize_Implementation()
{
	if (AbilitySystemComponentPrivate.IsValid() && OnAnyTagChangeDelegateHandle.IsValid())
	{
		FGameplayTagCountContainer& GameplayTagCountContainer = VS_PRIVABLIC_MEMBER(AbilitySystemComponentPrivate.Get(), UAbilitySystemComponent, GameplayTagCountContainer);
		VS_PRIVABLIC_MEMBER(&GameplayTagCountContainer, FGameplayTagCountContainer, OnAnyTagChangeDelegate).Remove(OnAnyTagChangeDelegateHandle);
	}
	
	Super::Uninitialize_Implementation();
}

void UVSGameplayTagController::Tick_Implementation(float DeltaTime)
{
	Super::Tick_Implementation(DeltaTime);

	if (bNotifyTagsUpdateDuringTicks && IsDirty()) { NotifyTagsUpdated(); }
}

UAbilitySystemComponent* UVSGameplayTagController::GetAbilitySystemComponent()
{
	return AbilitySystemComponentPrivate.IsValid()
		? AbilitySystemComponentPrivate.Get()
		: UVSActorLibrary::GetAbilitySystemComponentFormActor(GetOwnerActor());
}

void UVSGameplayTagController::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponentPrivate.IsValid()) { AbilitySystemComponentPrivate->GetOwnedGameplayTags(TagContainer); }
}

bool UVSGameplayTagController::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasMatchingGameplayTag(TagToCheck) : false;
}

bool UVSGameplayTagController::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasAllMatchingGameplayTags(TagContainer) : false;
}

bool UVSGameplayTagController::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->HasAnyMatchingGameplayTags(TagContainer) : false;
}

bool UVSGameplayTagController::MatchesGameplayTagQuery(const FGameplayTagQuery& TagQuery) const
{
	return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->MatchesGameplayTagQuery(TagQuery) : false;
}

int32 UVSGameplayTagController::GetTagCount(FGameplayTag TagToCheck) const
{
	return AbilitySystemComponentPrivate.IsValid() ? AbilitySystemComponentPrivate->GetTagCount(TagToCheck) : 0;
}

void UVSGameplayTagController::AddTag(const FGameplayTag& GameplayTag, int32 Count)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	AbilitySystemComponentPrivate->AddLooseGameplayTag(GameplayTag, Count);
}

void UVSGameplayTagController::AddTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	AbilitySystemComponentPrivate->AddLooseGameplayTags(GameplayTags, Count);
}

void UVSGameplayTagController::RemoveTag(const FGameplayTag& GameplayTag, int32 Count)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	AbilitySystemComponentPrivate->RemoveLooseGameplayTag(GameplayTag, Count);
}

void UVSGameplayTagController::RemoveTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	AbilitySystemComponentPrivate->RemoveLooseGameplayTags(GameplayTags, Count);
}

void UVSGameplayTagController::SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	AbilitySystemComponentPrivate->SetTagMapCount(GameplayTag, NewCount);
}

void UVSGameplayTagController::AddReplicatedTag(const FGameplayTag& GameplayTag)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	SetTagCount(GameplayTag, 1);
	AbilitySystemComponentPrivate->AddReplicatedLooseGameplayTag(GameplayTag);
}

void UVSGameplayTagController::AddReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray()) { SetTagCount(GameplayTag, 1); }
	AbilitySystemComponentPrivate->AddReplicatedLooseGameplayTags(GameplayTags);
}

void UVSGameplayTagController::RemoveReplicatedTag(const FGameplayTag& GameplayTag)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	SetTagCount(GameplayTag, 0);
	AbilitySystemComponentPrivate->RemoveReplicatedLooseGameplayTag(GameplayTag);
}

void UVSGameplayTagController::RemoveReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray()) { SetTagCount(GameplayTag, 0); }
	AbilitySystemComponentPrivate->RemoveReplicatedLooseGameplayTags(GameplayTags);
}

void UVSGameplayTagController::NotifyTagsUpdated(bool bAllowCleanNotify)
{
	if (!bAllowCleanNotify && !IsDirty()) return;
	bTagsDirty = false;
	OnTagsUpdated.Broadcast();
}

void UVSGameplayTagController::NotifyTagEvent(const FGameplayTag& TagEvent)
{
	OnTagEventNotified.Broadcast(TagEvent);
}

void UVSGameplayTagController::OnAnyTagChange(const FGameplayTag Tag, int32 Count)
{
	OnTagNewOrClear.Broadcast(Tag, Count > 0);
	if (bNotifyTagsUpdateInstantly) { NotifyTagsUpdated(); }
	else { bTagsDirty = true; }
}
