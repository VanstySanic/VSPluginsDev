// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/VSTickFunctionOwnerInterface.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSTickableObject.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "Tickable Object")
class VSPLUGINSCORE_API UVSTickableObject : public UObject, public IVSTickFunctionOwnerInterface
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickExecuteSighature, float, DeltaTime);

public:
	virtual FTickFunction* GetTickFunctionPtr() const override { return const_cast<FVSObjectTickFunction*>(&PrimaryObjectTick); }
	
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void RegisterTickFunction();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void UnregisterTickFunction();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	bool IsTickFunctionRegistered() const;

	/** Tick function will execute only if this method returns true. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tick")
	bool CanTick() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Tick")
	void Tick(float DeltaTime);

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FTickExecuteSighature OnTick;
	
public:
	UPROPERTY(EditAnywhere, Category = "Tick")
	FVSObjectTickFunction PrimaryObjectTick;

	/** If false, the tick function won't register. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tick")
	bool bCanEverTicking = true;
};
