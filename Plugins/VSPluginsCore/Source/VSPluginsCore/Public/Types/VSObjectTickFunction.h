// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "Containers/Union.h"
#include "VSObjectTickFunction.generated.h"

/**
 * The struct allows user to register a tick function for a common uobject.
 */
USTRUCT()
struct VSPLUGINSCORE_API FVSObjectTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

	DECLARE_DELEGATE_ThreeParams(FObjectTickExecuteDelegate, float /*DeltaTime*/, ELevelTick /*TickType*/, FVSObjectTickFunction* /*ThisTickFunction*/);
	DECLARE_DELEGATE_RetVal(bool, FObjectTickAbilityDelegate);

	virtual ~FVSObjectTickFunction() override;
	
	/** 
	 * Abstract function actually execute the tick. 
	 * @param DeltaTime - frame time to advance, in seconds
	 * @param TickType - kind of tick for this frame
	 * @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
	 * @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completion of this task until certain child tasks are complete.
	 */
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
	
	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph. */
	virtual FString DiagnosticMessage() override;
	virtual FName DiagnosticContext(bool bDetailed) override;

	/** Register the tick function for specified object and do the setup process. */
	void RegisterAndSetup(UObject* InTargetObject);

	/** Unregister the tick function and do the cleanup. */
	void UnregisterAndCleanup();

public:
	/** If not bound, returns as true. */
	FObjectTickAbilityDelegate CanExecuteTick;

	/** Execute when ticked. */
	FObjectTickExecuteDelegate OnExecuteTick;
	
private:
	TWeakObjectPtr<UObject> Target;
	TUnion<TWeakObjectPtr<UObject>, TWeakObjectPtr<UActorComponent>, TWeakObjectPtr<AActor>> TypedOuter;
};

template<>
struct TStructOpsTypeTraits<FVSObjectTickFunction> : public TStructOpsTypeTraitsBase2<FVSObjectTickFunction>
{
	enum
	{
		WithCopy = false
	};
};