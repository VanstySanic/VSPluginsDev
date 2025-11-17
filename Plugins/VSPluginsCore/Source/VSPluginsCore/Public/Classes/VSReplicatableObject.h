// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSReplicatableObject.generated.h"

/**
 * UVSReplicatableObject
 *
 * A lightweight UObject base class that supports Unreal's network replication
 * system. Designed for creating instanced logic or data objects that replicate
 * alongside their owning Actor or Component.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Enables RPCs and property replication for UObjects.
 * - Uses the owning Actor's networking context for routing remote calls.
 * - Supports creation/destruction via network replication (OnCreatedFromReplication /
 *   OnDestroyedFromReplication).
 * - Provides a simple API (SetIsReplicated / GetIsReplicated) for enabling or
 *   disabling replication at runtime.
 * - Registers as a replicated subobject when replication is enabled.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * - Derive your UObject from UVSReplicatableObject.
 * - Place it inside a replicated Actor or Component (instanced or created at runtime).
 * - Call SetIsReplicated(true) to mark it for replication.
 * - Implement replicated properties or RPCs as with normal Actor subobjects.
 * - Optionally override OnCreatedFromReplication / OnDestroyedFromReplication
 *   to handle lifecycle events.
 *
 * -------------------------------------------------------------------------
 * Notes
 * -------------------------------------------------------------------------
 * - Requires bReplicateUsingRegisteredSubObjectList set to true inside the features' owner component / actor.
 */

UCLASS(Abstract, Blueprintable, BlueprintType)
class VSPLUGINSCORE_API UVSReplicatableObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface.

	//~ Begin IInterface_ActorSubobject Interface
	// We're using the same API as IInterface_ActorSubobject, but not using it directly as a size and performance optimization.
	virtual void OnCreatedFromReplication();
	virtual void OnDestroyedFromReplication();
	//~ End IInterface_ActorSubobject Interface
	
	/** Set whether this object should replicate or is replicating. */
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void SetIsReplicated(bool bShouldReplicate);

	/** Get whether this object should replicate or is replicating. */
	UFUNCTION(BlueprintCallable, Category = "Replication")
	bool GetIsReplicated() const { return bReplicates; }

	/** Called when we want to start replicating this object, should not be called explicitly. */
	UFUNCTION(BlueprintCallable, Category = "Replication")
	virtual void BeginReplication();

	/** Tell object to end replication, should not be called explicitly. */
	UFUNCTION(BlueprintCallable, Category = "Replication")
	virtual void EndReplication();
	
private:
	/**
	 * Whether this object currently replicating and should the network code consider it for replication.
	 * Owner actor, component and object must be replicating first.
	 */
	UPROPERTY(EditAnywhere, Replicated, Category = "Replication")
	uint8 bReplicates : 1;

	TWeakObjectPtr<AActor> OuterActorPrivate;
};
