// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Features/VSCharacterMovementFeature.h"
#include "Types/VSCharacterMovementTypes.h"
#include "Types/VSGameplayTypes.h"
#include "VSChrMovFeature_WalkingMovement.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Feature.ChrMov.Movement.Walking")
class VSMOVEMENTSYSTEM_API UVSChrMovFeature_WalkingMovement : public UVSCharacterMovementFeature
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStanceChangedSignature, const FGameplayTag&, NewStance, const FGameplayTag&, PrevStance);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGaitChangedSignature, const FGameplayTag&, NewGait, const FGameplayTag&, PrevGait);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void OnMovementTagEventNotified_Implementation(const FGameplayTag& TagEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsWalkingMode() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsPrevWalkingMode() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsCrouching() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetStance() const { return MovementData.Stance; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FGameplayTag GetPrevStance() const { return MovementData.PrevStance; }

	/**
	 * Get the gait for the specified stance.
	 * If InStance is empty, return the gait in the current stance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "InStance"))
	FGameplayTag GetGait(const FGameplayTag& InStance = FGameplayTag()) const;

	/**
	 * Get the prev gait for the specified stance.
	 * If InStance is empty, return the gait in the current stance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "InStance"))
	FGameplayTag GetPrevGait(const FGameplayTag& InStance = FGameplayTag()) const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "InStance"))
	void SetStance(const FGameplayTag& InStance);

	/**
	 * Set the gait for the specified stance.
	 * If InStance is empty, set the gait for the current stance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement", meta = (AutoCreateRefTerm = "InGait, InStance"))
	void SetGait(const FGameplayTag& InGait, const FGameplayTag& InStance = FGameplayTag());


private:
	void ListenToCrouchState();
	void RefreshHalfHeight();
	void ApplyMovementBaseSettings(const FVSMovementBaseSettings& Settings);
	void RefreshMovementBaseSettings(const FGameplayTag& TagEvent);

	UFUNCTION(Server, Reliable)
	void SetStance_Server(const FGameplayTag& InStance);

	UFUNCTION(Server, Reliable)
	void SetGait_Server(const FGameplayTag& InGait, const FGameplayTag& InStance = FGameplayTag());
	
	UFUNCTION(NetMulticast, Reliable)
	void SetGait_Multicast(const FGameplayTag& InGait, const FGameplayTag& InStance);
	
	UFUNCTION()
	void OnRep_ReplicatedStance();
	
	void SetStanceInternal(const FGameplayTag& InStance);
	void SetGaitInternal(const FGameplayTag& InGait, const FGameplayTag& InStance);
	
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, Category = "Movement")
	FOnStanceChangedSignature OnStanceChanged;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable, Category = "Movement")
	FOnGaitChangedSignature OnGaitChanged;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FGameplayTag DefaultStance;

	/** Stance will be set to that after the character is uncrouched by default. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FGameplayTag DefaultUncrouchedStance;
	
	/** <Stance, Gait> */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TMap<FGameplayTag, FGameplayTag> DefaultGaits;

	UPROPERTY(EditAnywhere, Category = "Movement")
	TMap<FGameplayTag, float> StancedHalfHeights;

	/** Used when movement settings is refreshed and no valid one is selected. */
	UPROPERTY(EditAnywhere, Category = "Movement")
	FVSMovementBaseSettings DefaultMovementBaseSettings;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	TMap<FVSMovementBaseSettings, FVSGameplayTagEventQuery> QueriedMovementBaseSettings;

	/** Works as an entry to define when to refresh the tagged movement base settings. */
	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<FVSGameplayTagEventQuery> RefreshMovementBaseSettingsQueries;
	
private:
	UPROPERTY(ReplicatedUsing = "OnRep_ReplicatedStance")
	FGameplayTag ReplicatedStance;
	
	struct FMovementData
	{
		FGameplayTag Stance;
		FGameplayTag PrevStance;
		/** <Stance, Gait> */
		TMap<FGameplayTag, FGameplayTag> Gaits;
		TMap<FGameplayTag, FGameplayTag> PrevGaits;
		FVSMovementBaseSettings CurrentMovementBaseSettings;

		/** Only use once and reset. */
		FGameplayTag DesiredUncrouchedStance = FGameplayTag::EmptyTag;
		float RelativeOffsetToMeshZ = 0.f;
	} MovementData;
};
