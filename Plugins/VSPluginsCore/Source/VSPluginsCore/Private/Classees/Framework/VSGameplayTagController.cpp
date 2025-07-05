// Copyright VanstySanic. All Rights Reserved.

#include "Classees/Framework/VSGameplayTagController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "Libraries/VSPrivablicLibrary.h"
#include "Types/VSGameplayTypes.h"

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

#if WITH_EDITORONLY_DATA
	if (bPrintDebugString)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, ToDebugString(), false);
	}
#endif
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

FGameplayTagContainer UVSGameplayTagController::GetGameplayTags() const
{
	FGameplayTagContainer GameplayTagContainer;
	GetOwnedGameplayTags(GameplayTagContainer);
	return GameplayTagContainer;
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
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetReplicatedTagExists_Server(GameplayTag, true);
	}
	else
	{
		SetTagCount(GameplayTag, 1);
	}
}

void UVSGameplayTagController::AddReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetReplicatedTagsExist_Server(GameplayTags, true);
	}
	else
	{
		for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray()) { SetTagCount(GameplayTag, 1); }
	}
}

void UVSGameplayTagController::RemoveReplicatedTag(const FGameplayTag& GameplayTag)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetReplicatedTagExists_Server(GameplayTag, false);
	}
	else
	{
		SetTagCount(GameplayTag, 0);
	}
}

void UVSGameplayTagController::RemoveReplicatedTags(const FGameplayTagContainer& GameplayTags)
{
	if (!AbilitySystemComponentPrivate.IsValid()) { return; }
	if (UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetReplicatedTagsExist_Server(GameplayTags, false);
	}
	else
	{
		for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray()) { SetTagCount(GameplayTag, 0); }
	}
}

void UVSGameplayTagController::NotifyTagsUpdated(bool bAllowCleanNotify, bool bMulticast)
{
	if (!bAllowCleanNotify && !IsDirty()) return;
	if (bMulticast && UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		NotifyTagsUpdated_Server(bAllowCleanNotify);
	}
	else
	{
		bTagsDirty = false;
		NotifyTagEvent(EVSGameplayTagControllerTags::Event_TagsUpdated);
		OnTagsUpdated.Broadcast();
	}
}

void UVSGameplayTagController::NotifyTagEvent(const FGameplayTag& TagEvent, bool bMulticast)
{
	if (bMulticast && UVSActorLibrary::IsActorLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		NotifyTagEvent(TagEvent);
	}
	else
	{
		OnTagEventNotified.Broadcast(TagEvent);
	}
}

#if WITH_EDITORONLY_DATA
FString UVSGameplayTagController::ToDebugString()
{
	FString String = "Tags in actor " + GetOwnerActor()->GetName() + " ("
		+ UEnum::GetValueAsString(GetOwnerActor()->GetLocalRole()) + " / "
		+ UEnum::GetValueAsString(GetOwnerActor()->GetRemoteRole()) + "):";
	FGameplayTagContainer GameplayTags;
	GetOwnedGameplayTags(GameplayTags);
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray())
	{
		String += "\n\t" + GameplayTag.ToString();
	}
	return String;
}
#endif

UAbilitySystemComponent* UVSGameplayTagController::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPrivate.IsValid()
		? AbilitySystemComponentPrivate.Get()
		: UVSActorLibrary::GetAbilitySystemComponentFormActor(GetOwnerActor());
}

void UVSGameplayTagController::OnAnyTagChange(const FGameplayTag Tag, int32 Count)
{
	OnTagNewOrClear.Broadcast(Tag, Count > 0);
	if (bNotifyTagsUpdateInstantly) { NotifyTagsUpdated(); }
	else { bTagsDirty = true; }
}

void UVSGameplayTagController::SetReplicatedTagExists_Server_Implementation(const FGameplayTag& GameplayTag, bool bExists)
{
	SetTagCount(GameplayTag, bExists ? 1 : 0);
	AbilitySystemComponentPrivate->SetReplicatedLooseGameplayTagCount(GameplayTag, bExists ? 1 : 0);
}

void UVSGameplayTagController::SetReplicatedTagsExist_Server_Implementation(const FGameplayTagContainer& GameplayTags, bool bExists)
{
	for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray())
	{
		SetTagCount(GameplayTag, bExists ? 1 : 0);
		AbilitySystemComponentPrivate->SetReplicatedLooseGameplayTagCount(GameplayTag, bExists ? 1 : 0);
	}
}

void UVSGameplayTagController::NotifyTagsUpdated_Server_Implementation(bool bAllowCleanNotify)
{
	NotifyTagsUpdated_Multicast(bAllowCleanNotify);
}

void UVSGameplayTagController::NotifyTagsUpdated_Multicast_Implementation(bool bAllowCleanNotify)
{
	NotifyTagsUpdated(bAllowCleanNotify);
}

void UVSGameplayTagController::NotifyTagEvent_Server_Implementation(const FGameplayTag& TagEvent)
{
	NotifyTagEvent_Multicast(TagEvent);
}

void UVSGameplayTagController::NotifyTagEvent_Multicast_Implementation(const FGameplayTag& TagEvent)
{
	NotifyTagEvent(TagEvent, false);
}
