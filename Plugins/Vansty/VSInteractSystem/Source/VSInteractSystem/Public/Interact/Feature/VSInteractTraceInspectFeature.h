// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSInteractFeature.h"
#include "VSInteractTraceInspectFeature.generated.h"

class UVSControlRotationFeature;
/**
 * 
 */
UCLASS(DisplayName = "Feature.Interact.Inspect.Trace")
class VSINTERACTSYSTEM_API UVSInteractTraceInspectFeature : public UVSInteractFeature
{
	GENERATED_UCLASS_BODY()

public:

protected:
	virtual void Initialize_Implementation() override;
	virtual bool CanTick_Implementation() const override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void OnFeatureDeactivated_Implementation() override;

private:
	void DoTrace(float DeltaTime);
	TArray<UVSInteractiveFeatureAgent*> GetCurrentInspectiveTargets() const;

	
public:
	/** Empty as pass. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	FGameplayTagQuery DoTraceEntryTagQuery;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float MaxTraceDistance = 256.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	FVector2D MaxTraceYawAngleRange = FVector2D(-60.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	FVector2D MaxTracePitchAngleRange = FVector2D(-60.f, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float TraceAngleStep = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	int32 MaxInspectionNum = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	bool bSortByAngleInsteadOfDistance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	bool bTryUseControlDirectionInsteadOfActor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	bool bControlDirectionUseOnly2D = true;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugShapes = false;
#endif

private:
	TArray<TWeakObjectPtr<UVSInteractiveFeatureAgent>> TracedInspectiveTargets;
	TWeakObjectPtr<UVSControlRotationFeature> ControlRotationFeaturePrivate;
};
