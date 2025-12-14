// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSReplicatableObject.generated.h"

/**
 * Base UObject that supports Unreal Engine network replication.
 *
 * This class enables property replication and RPC execution for instanced
 * UObject types that are owned by, and replicate alongside, an Actor or
 * ActorComponent. Replication is routed through the owning object's
 * networking context and lifecycle.
 *
 * Instances may be created or destroyed via network replication and can
 * override replication lifecycle callbacks to react to those events.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VSPLUGINSCORE_API UVSReplicatableObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual void PostInitProperties() override;
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
