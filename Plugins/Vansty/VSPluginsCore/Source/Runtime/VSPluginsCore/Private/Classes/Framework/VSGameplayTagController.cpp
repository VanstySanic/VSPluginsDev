// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Framework/VSGameplayTagController.h"
#include "AbilitySystemComponent.h"
#include "Libraries/VSActorLibrary.h"
#include "VSPrivablic.h"
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
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
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
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
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
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
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
	if (UVSActorLibrary::IsActorNetLocalRoleAuthorityOrAutonomous(GetOwnerActor()) && GetIsReplicated())
	{
		SetReplicatedTagsExist_Server(GameplayTags, false);
	}
	else
	{
		for (const FGameplayTag& GameplayTag : GameplayTags.GetGameplayTagArray()) { SetTagCount(GameplayTag, 0); }
	}
}

void UVSGameplayTagController::NotifyTagsUpdated(bool bAllowCleanNotify, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			NotifyTagsUpdatedInternal(bAllowCleanNotify);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			NotifyTagsUpdated_Server(bAllowCleanNotify, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		NotifyTagsUpdated_Server(bAllowCleanNotify, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			NotifyTagsUpdatedInternal(bAllowCleanNotify);
		}
	}
}

void UVSGameplayTagController::NotifyTagEvent(const FGameplayTag& TagEvent, const FVSNetMethodExecutionPolicies& NetExecPolicies)
{
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			NotifyTagEventInternal(TagEvent);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			NotifyTagEvent_Server(TagEvent, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		NotifyTagEvent_Server(TagEvent, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			NotifyTagEventInternal(TagEvent);
		}
	}
}

FString UVSGameplayTagController::ToDebugString()
{
#if WITH_EDITORONLY_DATA
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
#else
	return FString();
#endif
}

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

void UVSGameplayTagController::NotifyTagsUpdated_Server_Implementation(bool bAllowCleanNotify, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
    {
    	NotifyTagsUpdatedInternal(bAllowCleanNotify);
    }
    if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
    {
    	NotifyTagsUpdated_Multicast(bAllowCleanNotify, NetExecPolicy);
    }
}

void UVSGameplayTagController::NotifyTagsUpdated_Multicast_Implementation(bool bAllowCleanNotify, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetOwnerActor()->HasAuthority()) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }

	if (bShouldExecute)
	{
		NotifyTagsUpdatedInternal(bAllowCleanNotify);
	}
}

void UVSGameplayTagController::NotifyTagsUpdatedInternal(bool bAllowCleanNotify)
{
	if (!bAllowCleanNotify && !IsDirty()) return;
	bTagsDirty = false;
	NotifyTagEvent(EVSGameplayTagControllerTags::Event_TagsUpdated);
	OnTagsUpdated.Broadcast();
}

void UVSGameplayTagController::NotifyTagEventInternal(const FGameplayTag& TagEvent)
{
	OnTagEventNotified.Broadcast(TagEvent);
}

void UVSGameplayTagController::NotifyTagEvent_Server_Implementation(const FGameplayTag& TagEvent, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		NotifyTagEventInternal(TagEvent);
	}
	if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
	{
		NotifyTagEvent_Multicast(TagEvent, NetExecPolicy);
	}
}

void UVSGameplayTagController::NotifyTagEvent_Multicast_Implementation(const FGameplayTag& TagEvent, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetOwnerActor()->HasAuthority()) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }

	if (bShouldExecute)
	{
		NotifyTagEventInternal(TagEvent);
	}
}

