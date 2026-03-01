// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "Classes/VSObjectFeature.h"
#include "Interfaces/VSGameplayTagFeatureInterface.h"
#include "Types/VSTreeQueryTypes.h"
#include "VSEnhancedInputContextBinderFeature.generated.h"

class UVSGameplayTagFeatureBase;

/** Per-context rules used by UVSEnhancedInputContextBinderFeature. */
USTRUCT(BlueprintType)
struct FVSEnhancedInputContextBinderSettings
{
	GENERATED_BODY()

	FVSEnhancedInputContextBinderSettings()
		: bAddByDefault(false)
	{
	}
	
	/** Priority passed to AddMappingContext. Higher values win conflicts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AddPriority = 0;

	/** Adds the context when this query matches incoming tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoAddTagQuery;

	/** Removes the context when this query matches incoming tag events and current tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQuery AutoRemoveTagQuery;
	
	/** Options used when adding the context. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FModifyContextOptions AddModifyOptions;

	/** Options used when removing the context. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FModifyContextOptions RemoveModifyOptions;

	/** If true, add this mapping context by default before query refresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bAddByDefault : 1;
};

/**
 * Auto-manages Enhanced Input mapping contexts for local players using gameplay tag events.
 * Adds default contexts on BeginPlay, then applies add/remove rules driven by tag queries.
 * Only runs for locally controlled actors and uses the owning gameplay tag feature as the source.
 */
UCLASS(DisplayName = "VS.Feature.Input.Binder.Context")
class VSPLUGINSCORE_API UVSEnhancedInputContextBinderFeature : public UVSObjectFeature, public IVSGameplayTagFeatureInterface
{
	GENERATED_UCLASS_BODY()


protected:
	//~ Begin UVSObjectFeature Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BeginPlay_Implementation() override;
	
	//~ End IVSGameplayTagFeatureInterface Interface
	virtual UVSGameplayTagFeatureBase* GetPrimaryGameplayTagFeature_Implementation() const override;
	virtual void OnGameplayTagFeatureTagsUpdated_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature) override;
	virtual void OnGameplayTagFeatureTagEventsNotified_Implementation(UVSGameplayTagFeatureBase* GameplayTagFeature, const FGameplayTagContainer& TagEvents) override;
	//~ Begin IVSGameplayTagFeatureInterface Interface

private:
	void RefreshContexts(const FGameplayTagContainer& TagEvents);
	UEnhancedInputLocalPlayerSubsystem* GetLocalPlayerSubsystem() const;
	
protected:
	/** Mapping contexts and their add/remove rules. */
	UPROPERTY(EditAnywhere, Category = "Input")
	TMap<TObjectPtr<UInputMappingContext>, FVSEnhancedInputContextBinderSettings> ContextSettings;
	
protected:
	TWeakObjectPtr<UVSGameplayTagFeatureBase> GameplayTagFeaturePrivate;
	TWeakObjectPtr<APlayerController> PlayerControllerPrivate;
	/** Contexts currently added by this feature instance. */
	TSet<TObjectPtr<UInputMappingContext>> AddedContexts;
};
