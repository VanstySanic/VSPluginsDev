// Copyright VanstySanic. All Rights Reserved.

#include "Interact/Feature/VSInteractFeatureAgent.h"
#include "Classes/Framework/VSGameplayTagController.h"
#include "Interactive/Feature/VSInteractiveFeatureAgent.h"
#include "Interactive/Feature/Action/VSInteractiveActionFeature.h"
#include "Libraries/VSActorLibrary.h"
#include "Types/VSInteractSystemTags.h"

UVSInteractFeatureAgent::UVSInteractFeatureAgent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSInteractFeatureAgent::Initialize_Implementation()
{
	Super::Super::Initialize_Implementation();

	InteractFeatureAgentPrivate = this;
	
	GameplayTagControllerPrivate = UVSActorLibrary::GetGameplayTagControllerFromActor(GetOwnerActor());
	check(GameplayTagControllerPrivate.IsValid());
}

void UVSInteractFeatureAgent::EndPlay_Implementation()
{
	if (IsInspecting() && CurrentInspectivePrivate.IsValid())
	{
		StopInspectionOnTarget(CurrentInspectivePrivate.Get());
	}

	if (IsInteracting() && CurrentInteractivePrivate.IsValid())
	{
		StopInteractionWithTarget(CurrentInteractivePrivate.Get(), CurrentInteractiveActionFeatureName);
	}
	
	Super::EndPlay_Implementation();
}

void UVSInteractFeatureAgent::TryInspectTarget(UVSInteractiveFeatureAgent* TargetAgent)
{
	if (!TargetAgent || !TargetAgent->IsInteractable(this) || IsInteracting()) return;
	if (IsInspecting() && CurrentInspectivePrivate == TargetAgent) return;

	if (IsInspecting() && CurrentInspectivePrivate.IsValid())
	{
		StopInspectionOnTarget(CurrentInspectivePrivate.Get());
	}
	
	CurrentInspectivePrivate = TargetAgent;
	TargetAgent->OnInspectBySource(this);

	OnInspectionStart.Broadcast(TargetAgent);
}

void UVSInteractFeatureAgent::StopInspectionOnTarget(UVSInteractiveFeatureAgent* TargetAgent)
{
	if (!TargetAgent || !TargetAgent->IsInspectedBySource(this)) return;
	if (CurrentInspectivePrivate == TargetAgent)
	{
		CurrentInspectivePrivate.Reset();
	}
	TargetAgent->OnStopInspectBySource(this);
	OnInspectionEnd.Broadcast(TargetAgent);
}

void UVSInteractFeatureAgent::TryInteractWithTarget(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName)
{
	if (!TargetAgent || !TargetAgent->IsInteractable(this)) return;
	UVSInteractiveActionFeature* ActionFeature = !ActionFeatureName.IsNone()
		? TargetAgent->GetActionFeatureByName(ActionFeatureName)
		: TargetAgent->FindSubFeatureByClass<UVSInteractiveActionFeature>();
	if (!ActionFeature || !ActionFeature->IsAvailableForSource(this)) return;
	
	const FVSNetMethodExecutionPolicies& NetExecPolicies = ActionFeature->NetExecutionPolicies;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			const bool bSucceeded = TryInteractWithTargetInternal(TargetAgent, ActionFeature->FeatureName);
			if (bSucceeded && NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
			{
				/** Only send server RPC if local execution succeeded.  */
				InteractWithTarget_Server(TargetAgent, ActionFeature->FeatureName, NetExecPolicies.ServerRPCPolicy);
			}
		}
		else if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			TryInteractWithTarget_Server(TargetAgent, ActionFeature->FeatureName, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		TryInteractWithTarget_Server(TargetAgent, ActionFeature->FeatureName, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			TryInteractWithTargetInternal(TargetAgent, ActionFeature->FeatureName);
		}
	}
}

void UVSInteractFeatureAgent::StopInteractionWithTarget(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, bool bIsLocalBreak)
{
	if (!TargetAgent || !TargetAgent->IsInteractedBySource(this)) return;
	UVSInteractiveActionFeature* ActionFeature = !ActionFeatureName.IsNone()
		? TargetAgent->GetActionFeatureByName(ActionFeatureName)
		: TargetAgent->FindSubFeatureByClass<UVSInteractiveActionFeature>();
	if (!ActionFeature || !ActionFeature->IsInteractedBySource(this)) return;

	if (bIsLocalBreak)
	{
		StopInteractWithTargetInternal(TargetAgent, ActionFeatureName);
		return;
	}
	
	const FVSNetMethodExecutionPolicies& NetExecPolicies = ActionFeature->NetExecutionPolicies;
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Client)
		{
			StopInteractWithTargetInternal(TargetAgent, ActionFeatureName);
		}
		if (NetExecPolicies.AutonomousLocalPolicy & EVSNetAutonomousMethodExecPolicy::Server)
		{
			StopInteractionWithTarget_Server(TargetAgent, ActionFeatureName, NetExecPolicies.ServerRPCPolicy);
		}
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_Authority)
	{
		StopInteractionWithTarget_Server(TargetAgent, ActionFeatureName, NetExecPolicies.AuthorityLocalPolicy);
	}
	else if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (NetExecPolicies.bSimulatedLocalExecution)
		{
			StopInteractWithTargetInternal(TargetAgent, ActionFeatureName);
		}
	}
}

bool UVSInteractFeatureAgent::TryInteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName)
{
	if (!TargetAgent || !TargetAgent->IsInteractable(this)) return false;
	UVSInteractiveActionFeature* ActionFeature = TargetAgent->GetActionFeatureByName(ActionFeatureName);
	if (!ActionFeature || !ActionFeature->IsAvailableForSource(this)) return false;

	InteractWithTargetInternal(TargetAgent, ActionFeatureName);
	return true;
}

void UVSInteractFeatureAgent::InteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName)
{
	if (!TargetAgent) return;

	/** End inspection when interacting with target.. */
	if (IsInspecting() && CurrentInspectivePrivate.IsValid())
	{
		StopInspectionOnTarget(CurrentInspectivePrivate.Get());
	}
	
	CurrentInteractivePrivate = TargetAgent;
	CurrentInteractiveActionFeatureName = ActionFeatureName;
	TargetAgent->OnInteractBySource(this, ActionFeatureName);

	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(EVSInteractState::Interacting, 1);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSInteractEvent::Interact_Start);

	OnInteractionStart.Broadcast(TargetAgent, ActionFeatureName);

	UVSInteractiveActionFeature* ActionFeature = TargetAgent->GetActionFeatureByName(ActionFeatureName);
	if (ActionFeature->bIsInstantAction)
	{
		StopInteractionWithTarget(TargetAgent, ActionFeatureName, true);
	}
}

void UVSInteractFeatureAgent::StopInteractWithTargetInternal(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName)
{
	if (!TargetAgent) return;
	if (TargetAgent == CurrentInteractivePrivate.Get())
	{
		CurrentInteractivePrivate.Reset();
		if (CurrentInteractiveActionFeatureName == ActionFeatureName)
		{
			CurrentInteractiveActionFeatureName = NAME_None;
		}
	}
	TargetAgent->OnStopInteractionBySource(this, ActionFeatureName);
	
	UVSGameplayTagController* GameplayTagController = GetGameplayTagController();
	GameplayTagController->SetTagCount(EVSInteractState::Interacting, 0);
	GameplayTagController->NotifyTagsUpdated();
	GameplayTagController->NotifyTagEvent(EVSInteractEvent::Interact_End);

	OnInteractionEnd.Broadcast(TargetAgent, ActionFeatureName);
}

void UVSInteractFeatureAgent::TryInteractWithTarget_Server_Implementation(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (!TargetAgent) return;
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		const bool bSuccessful = TryInteractWithTargetInternal(TargetAgent, ActionFeatureName);
		if (bSuccessful && NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			InteractWithTarget_Multicast(TargetAgent, ActionFeatureName, NetExecPolicy);
		}
	}
}

void UVSInteractFeatureAgent::InteractWithTarget_Server_Implementation(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (!TargetAgent) return;
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		InteractWithTargetInternal(TargetAgent, ActionFeatureName);
		if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			InteractWithTarget_Multicast(TargetAgent, ActionFeatureName, NetExecPolicy);
		}
	}
}

void UVSInteractFeatureAgent::InteractWithTarget_Multicast_Implementation(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetOwnerActor()->HasAuthority()) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		InteractWithTargetInternal(TargetAgent, ActionFeatureName);
	}
}

void UVSInteractFeatureAgent::StopInteractionWithTarget_Server_Implementation(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	if (!TargetAgent) return;
	if (NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Server)
	{
		StopInteractWithTargetInternal(TargetAgent, ActionFeatureName);
		if (NetExecPolicy > EVSNetAuthorityMethodExecPolicy::Server)
		{
			StopInteractionWithTarget_Multicast(TargetAgent, ActionFeatureName, NetExecPolicy);
		}
	}
}

void UVSInteractFeatureAgent::StopInteractionWithTarget_Multicast_Implementation(UVSInteractiveFeatureAgent* TargetAgent, const FName ActionFeatureName, EVSNetAuthorityMethodExecPolicy::Type NetExecPolicy)
{
	bool bShouldExecute = true;

	/** Authority already handled. */
	if (GetOwnerActor()->HasAuthority()) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_AutonomousProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Client)) { bShouldExecute = false; }
	if (GetOwnerActor()->GetLocalRole() == ROLE_SimulatedProxy && !(NetExecPolicy & EVSNetAuthorityMethodExecPolicy::Simulated)) { bShouldExecute = false; }
	
	if (bShouldExecute)
	{
		StopInteractWithTargetInternal(TargetAgent, ActionFeatureName);
	}
}
