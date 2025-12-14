// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "Containers/Union.h"
#include "VSObjectTickFunction.generated.h"

/**
 * FVSObjectTickFunction
 *
 * Custom FTickFunction implementation that enables UObjects to participate
 * in the engine tick system.
 *
 * This tick function mirrors the behavior of Actor and Component tick
 * functions, while being applicable to non-Actor, non-Component objects.
 * It resolves an appropriate outer context for ticking and forwards execution
 * to user-provided delegates.
 *
 * The tick function supports conditional execution and integrates with the
 * engine tick dependency graph and lifetime management.
 */
USTRUCT()
struct VSPLUGINSCORE_API FVSObjectTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

	DECLARE_DELEGATE_ThreeParams(FOnExecuteTickDelegate, float /*DeltaTime*/, ELevelTick /*TickType*/, FVSObjectTickFunction* /*ThisTickFunction*/);
	DECLARE_DELEGATE_RetVal(bool, FCanExecuteTickDelegate);

	
	FVSObjectTickFunction()
		: bTickCrossWorldIfPossible(false)
		, bShouldTickCrossWorld(false)
	{
	}

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

	/** Register the tick function for the given object and do the setup process. */
	void RegisterAndSetup(UObject* InTargetObject);

	/** Unregister the tick function and do the cleanup. */
	void UnregisterAndCleanup();

protected:
	/** Whether the object is explicitly world-bound (e.g. Actor, ActorComponent, WorldSubsystem, transient gameplay objects) */
	virtual bool IsWorldRelatedObject(const UObject* Object);

public:
	/** If not bound, returns as true. */
	FCanExecuteTickDelegate CanExecuteTick;

	/** Execute when ticking. */
	FOnExecuteTickDelegate OnExecuteTick;

	/**
	 * If bTickCrossWorldIfPossible is enabled and the target UObject is not explicitly world-bound,
	 * the tick function can re-register itself when new worlds are created or torn down.
	 * This is useful for systems that must tick even during world reloads, PIE transitions, or seamless travel.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Tick", AdvancedDisplay)
	uint8 bTickCrossWorldIfPossible : 1;
	
private:
	using FTickFunction::RegisterTickFunction;
	using FTickFunction::UnRegisterTickFunction;
	
	TWeakObjectPtr<UObject> Target;
	TUnion<TWeakObjectPtr<UObject>, TWeakObjectPtr<UActorComponent>, TWeakObjectPtr<AActor>> TypedTargetOuter;

	uint8 bShouldTickCrossWorld : 1;
	FDelegateHandle OnWorldInitializeDelegateHandle;
	FDelegateHandle OnWorldTearDownDelegateHandle;
};

template<>
struct TStructOpsTypeTraits<FVSObjectTickFunction> : public TStructOpsTypeTraitsBase2<FVSObjectTickFunction>
{
	enum
	{
		WithCopy = false
	};
};