// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/VSTickFunctionOwnerInterface.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSTickableObject.generated.h"

/**
 * Base UObject that participates in ticking through FVSObjectTickFunction.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VSPLUGINSCORE_API UVSTickableObject : public UObject, public IVSTickFunctionOwnerInterface
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
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	bool IsTickFunctionRegistered() const;

	/** Tick is executed only when this returns true. */
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
	/** Configurable primary tick settings for this object. */
	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	FVSObjectTickFunction PrimaryObjectTick;
};


/** ------------------------------------------------------------------------- **/


/**
 * Tick-forwarding object that exposes per-frame delegates for native and Blueprint consumers.
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

	/** Requires both native and Blueprint can-tick checks to pass. */
	virtual bool CanExecuteTick_Implementation() const override;

public:
	/** Optional native predicate; unbound means "allow tick". */
	FCanTickDelegate CanTick_Native;
	/** Broadcast every tick. */
	FOnTickDelegate OnTick_Native;

	/** Optional Blueprint predicate; unbound means "allow tick". */
	UPROPERTY(BlueprintReadOnly, DisplayName = "CanTick")
	FCanTickEvent CanTick;
	
	/** Broadcast every tick. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, DisplayName = "OnTick")
	FOnTickEvent OnTick;
};
