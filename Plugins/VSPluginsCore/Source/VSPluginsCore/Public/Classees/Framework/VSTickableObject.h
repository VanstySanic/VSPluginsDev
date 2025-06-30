// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSObjectTickFunction.h"
#include "UObject/Object.h"
#include "VSTickableObject.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "Tickable Object")
class VSPLUGINSCORE_API UVSTickableObject : public UObject
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTickExecuteSighature, float, DeltaTime);

public:
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void RegisterTickFunction();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Tick")
	void UnregisterTickFunction();

	/** Tick function will execute only when this returns true. */
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

	/** You can force disable the object tick function by set this to false. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tick")
	uint8 bAllowTicking : 1;
};
