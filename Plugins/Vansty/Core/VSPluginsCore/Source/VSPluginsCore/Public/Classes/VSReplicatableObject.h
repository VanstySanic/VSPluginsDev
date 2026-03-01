// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VSReplicatableObject.generated.h"

/**
 * Base UObject that supports property replication and RPC routing through an owning Actor context.
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
	
	/** Enables/disables replication participation for this object. */
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void SetIsReplicated(bool bShouldReplicate);

	/** Returns whether this object is currently marked to replicate. */
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
