// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "VSChrMovCapsuleComponent.generated.h"

/**
 * This capsule is especially designed for character movement.
 */
UCLASS(meta = (DisplayName="Character Movement Capsule", BlueprintSpawnableComponent))
class VSMOVEMENTSYSTEM_API UVSChrMovCapsuleComponent : public UCapsuleComponent
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UShapeComponent Interface
	virtual void UpdateBodySetup() override;
	//~ End UShapeComponent Interface

	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	FVector GetCapsuleCenterLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetCapsuleRootLocation() const;

	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	void SetCapsuleHalfHeightAndKeepRoot(float NewHalfHeight, bool bUpdateOverlaps = true);

	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	void SetCapsuleCenterOffsetZ(float CenterOffsetZ, bool bUpdateOverlaps = true);

	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	void AddCapsuleCenterOffsetZ(float DeltaCenterOffsetZ, bool bUpdateOverlaps = true);
	
	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	float GetScaledCapsuleCenterOffsetZ() const;

	UFUNCTION(BlueprintCallable, Category="Components|Capsule")
	float GetUnscaledCapsuleCenterOffsetZ() const { return CapsuleCenterOffsetZ; }
	
protected:
	/** Offset to adjust the shape location Z without changing the component location Z. Unscaled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, export, Category = "Shape")
	float CapsuleCenterOffsetZ = 0.f;
};
