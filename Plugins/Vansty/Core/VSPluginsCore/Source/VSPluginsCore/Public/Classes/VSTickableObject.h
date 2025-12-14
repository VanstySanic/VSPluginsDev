// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/VSTickFunctionInterface.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSTickableObject.generated.h"

/**
 * Base UObject that provides per-frame ticking support through a custom
 * FVSObjectTickFunction.
 *
 * This class allows non-Actor, non-Component UObject instances to participate
 * in the engine tick without requiring scene presence or world placement.
 * It encapsulates tick registration, execution, and latent action processing
 * in a lightweight, reusable form.
 *
 * Tick execution is driven by an internal FVSObjectTickFunction and ultimately
 * forwards to the Tick() virtual function, which may be implemented in C++ or Blueprint.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VSPLUGINSCORE_API UVSTickableObject : public UObject, public IVSTickFunctionInterface
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface.

	//~ Begin IVSTickFunctionInterface.
	virtual FTickFunction* GetPrimaryTickFunctionPtr() const override;
	//~ End IVSTickFunctionInterface.
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	virtual void RegisterTickFunction();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	virtual void UnregisterTickFunction();
	
	/** Whether the primary tick function is registered. */
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	bool IsTickFunctionRegistered() const;

	/** Tick function will execute only if this method returns true. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tick")
	bool CanExecuteTick() const;

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	bool IsTickFunctionEnabled() const;
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void SetTickFunctionEnabled(bool bEnabled);
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void SetTickGroup(ETickingGroup TickingGroup);
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void SetEndTickGroup(ETickingGroup TickingGroup);
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	float GetTickTimeInterval() const { return PrimaryObjectTick.TickInterval; }

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void SetTickTimeInterval(float Interval = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	bool GetTickableWhenPaused() const { return PrimaryObjectTick.bTickEvenWhenPaused; }

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void SetTickableWhenPaused(bool bEnabled);
	
protected:
	/**
	 * Function called every frame on this object.
	 * This will call the Tick method.
	 * Only executes if the tick function is registered and CanExecuteTick returns true.
	 * @param DeltaTime The time since the last tick.
	 * @param TickType The kind of tick this is, for example, are we paused, or 'simulating' in the editor
	 * @param TickFunction Internal tick function struct that caused this to run
	 */
	virtual void TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction);
	
	/**
	 * Function called every frame on this object.
	 * Override this function to implement custom logic to be executed every frame.
	 * Only executes if the tick function is registered and CanExecuteTick returns true.
	 * @param DeltaTime The time since the last tick.
	 * @remarks You might need to call the super native implementation manually in blueprints.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Tick")
	void Tick(float DeltaTime);

public:
	/** Main tick function for the object. */
	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	FVSObjectTickFunction PrimaryObjectTick;
};


/** ------------------------------------------------------------------------- **/


/**
 * UVSObjectTickProxy
 *
 * A lightweight tick-forwarding UObject that exposes per-frame updates
 * to both C++ and Blueprint. Intended for cases where Blueprint scripts
 * or external systems need tick behavior on a UObject without creating
 * additional native subclasses.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Forwards UVSTickableObject's tick into C++ and Blueprint delegates.
 * - Provides a native multicast tick delegate (OnTick) with full tick data.
 * - Provides a BlueprintAssignable tick event (EventOnTick) with DeltaTime.
 * - Includes an optional Blueprint-driven predicate (EventCanTick) that
 *   native code may execute to decide whether tick logic should proceed.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * - Create or instance this object inside an Actor or UObject.
 * - Call RegisterTickFunction() to start receiving ticks.
 * - Bind to OnTick (C++) or EventOnTick (Blueprint) for per-frame logic.
 * - Optionally bind EventCanTick in Blueprint and evaluate it from C++.
 * - Call UnregisterTickFunction() when ticking is no longer needed.
 */
UCLASS(NotBlueprintable, DefaultToInstanced, EditInlineNew)
class VSPLUGINSCORE_API UVSObjectTickProxy final : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_DELEGATE_RetVal_OneParam(bool, FCanTickDelegate, UVSObjectTickProxy* /** TickProxy */);
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanTickEvent, UVSObjectTickProxy*, TickProxy);
	DECLARE_MULTICAST_DELEGATE_FourParams(FOnTickDelegate, UVSObjectTickProxy* /** TickProxy */, float /** DeltaTime */, ELevelTick /** TickType */, FVSObjectTickFunction* /** TickFunction */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTickEvent, UVSObjectTickProxy*, TickProxy, float, DeltaTime);

public:
	virtual void TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction) override;

	/** Requires both CanTick_Native and CanTick return true. */
	virtual bool CanExecuteTick_Implementation() const override;

public:
	/** If not bound, returns as true. */
	FCanTickDelegate CanTick_Native;
	FOnTickDelegate OnTick_Native;

	/** If not bound, returns as true. */
	UPROPERTY(BlueprintReadOnly, DisplayName = "CanTick")
	FCanTickEvent CanTick;
	
	/** Event broadcast every tick. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, DisplayName = "OnTick")
	FOnTickEvent OnTick;
};