// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSTickableObject.generated.h"

/**
 * UVSTickableObject
 *
 * A tickable UObject that can participate in the engine's ticking system through a custom
 * FVSObjectTickFunction. Unlike Actors or Components, this class allows any UObject instance
 * to register a per-frame tick without requiring a scene component or world-placed actor.
 *
 * The primary goal of this class is to provide lightweight, extensible ticking behavior for
 * gameplay-related objects, data objects, or runtime managers that are not derived from
 * AActor or UActorComponent.
 *
 * -------------------------------------------------------------------------
 * Key Features
 * -------------------------------------------------------------------------
 * - Provides a native tick function via FVSObjectTickFunction.
 * - Allows dynamic registration and unregistration from the world tick.
 * - Supports Blueprint extension through BlueprintNativeEvent (Tick).
 * - Supports conditional ticking via CanExecuteTick().
 * - Integrates with the latent action system (ProcessLatentActions).
 *
 * -------------------------------------------------------------------------
 * Tick Flow
 * -------------------------------------------------------------------------
 *   World Tick → FVSObjectTickFunction → TickObject() → Tick() (Blueprint/C++)
 *
 * TickObject() is responsible for:
 *   - Calling the Blueprint/C++ Tick() function.
 *   - Processing latent actions bound to this object.
 *
 * -------------------------------------------------------------------------
 * Usage
 * -------------------------------------------------------------------------
 * 1. Derive from UVSTickableObject.
 * 2. Implement Tick() in C++ or Blueprint.
 * 3. Call RegisterTickFunction(Target) to enable ticking.
 * 4. Call UnregisterTickFunction() when ticking is no longer needed.
 *
 * -------------------------------------------------------------------------
 * Blueprint Notes
 * -------------------------------------------------------------------------
 * - Tick(float DeltaTime) is a BlueprintNativeEvent.
 *   The Blueprint implementation must call "Parent: Tick" manually if the
 *   C++ native behavior should also execute.
 *
 * -------------------------------------------------------------------------
 * Suitable For
 * -------------------------------------------------------------------------
 * - Standalone gameplay logic objects.
 * - Data-driven systems needing per-frame updates.
 * - Runtime managers or subsystems not suitable as Actors.
 * - Any UObject that needs ticking behavior.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VSPLUGINSCORE_API UVSTickableObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface.
	virtual UWorld* GetWorld() const override;
	//~ End UObject Interface.
	
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

	DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FCanTickEvent);
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTickDelegate, float /** DeltaTime */, ELevelTick /** TickType */, FVSObjectTickFunction* /** TickFunction */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickEvent, float, DeltaTime);

public:
	virtual void TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction) override;

public:
	FOnTickDelegate OnTick_Native;

	/** If not bound, returns as true. */
	UPROPERTY(BlueprintReadOnly, DisplayName = "CanTick")
	FCanTickEvent CanTick;
	
	/** Event broadcast every tick. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, DisplayName = "OnTick")
	FOnTickEvent OnTick;
};