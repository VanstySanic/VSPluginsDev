// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "Containers/Union.h"
#include "VSObjectTickFunction.generated.h"

/**
 * FVSObjectTickFunction
 *
 * A custom tick function that enables regular UObjects to participate in the engine's
 * tick system. This tick function behaves similarly to the built-in Actor and Component
 * tick functions, but is designed specifically for non-Actor, non-Component objects.
 *
 * The function can dynamically register or unregister itself with a world, resolve
 * the correct tick target (ActorComponent, Actor, or UObject), and forward the tick
 * to user-defined delegates. This allows any UObject to implement per-frame behavior
 * without requiring scene placement or inheritance from AActor.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Allows a plain UObject to register a tick into the engine tick pipeline.
 * - Automatically detects the closest outer AActor or UActorComponent for context.
 * - Supports conditional ticking through CanExecuteTick.
 * - Supports per-object time dilation when the tick target is an Actor or Component.
 * - Supports ticking across world transitions (if enabled).
 * - Provides diagnostic information for the tick dependency graph.
 * - Cleans up safely when the tick function or its target is destroyed.
 *
 * -------------------------------------------------------------------------
 * Tick Invocation Flow
 * -------------------------------------------------------------------------
 *   World Tick → FTickFunction → ExecuteTick() →
 *     → (Time dilation & validity checks)
 *     → OnExecuteTick delegate (user logic)
 *
 * The user logic is provided through delegates:
 *   - CanExecuteTick: Optional filter to determine whether ticking should occur.
 *   - OnExecuteTick: The callback invoked when the tick is executed.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * 1. Instantiate or embed this struct inside a UObject.
 * 2. Bind OnExecuteTick and CanExecuteTick as needed.
 * 3. Call RegisterAndSetup(this) to register with the current world.
 * 4. Call UnregisterAndCleanup() to unregister safely.
 *
 * This is typically used in conjunction with UVSTickableObject.
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