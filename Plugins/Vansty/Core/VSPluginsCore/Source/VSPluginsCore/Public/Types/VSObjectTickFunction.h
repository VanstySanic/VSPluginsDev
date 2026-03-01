// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "Containers/Union.h"
#include "VSObjectTickFunction.generated.h"

/**
 * Custom FTickFunction that enables UObject-based ticking with delegate dispatch.
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
	/** Returns true for objects whose tick lifetime is directly world-bound. */
	virtual bool IsWorldRelatedObject(const UObject* Object);

public:
	/** If not bound, returns as true. */
	FCanExecuteTickDelegate CanExecuteTick;

	/** Execute when ticking. */
	FOnExecuteTickDelegate OnExecuteTick;

	/**
	 * If bTickCrossWorldIfPossible is enabled and the target UObject is not explicitly world-bound,
	 * the tick function can re-register itself when new worlds are created or torn down.
	 * This is useful for systems that must tick even during world reloads, transitions, or seamless travel.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Tick", AdvancedDisplay)
	uint8 bTickCrossWorldIfPossible : 1;
	
private:
	using FTickFunction::RegisterTickFunction;
	using FTickFunction::UnRegisterTickFunction;
	
	TWeakObjectPtr<UObject> Target;
	TUnion<TWeakObjectPtr<UObject>, TWeakObjectPtr<UActorComponent>, TWeakObjectPtr<AActor>> TypedTargetOuter;
	TWeakObjectPtr<UGameInstance> CachedGameInstance;

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
