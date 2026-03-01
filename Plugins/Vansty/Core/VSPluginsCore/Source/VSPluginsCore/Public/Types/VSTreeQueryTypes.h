// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "VSTreeQueryTypes.generated.h"

class USceneComponent;

/**
 * Defines how multiple query results are evaluated together.
 */
UENUM(BlueprintType)
namespace EVSTreeQueryMatchRange
{
	enum Type
	{
		None,
		Any,
		All,
	};
}

/**
 * Defines the source type used by a query expression node.
 */
UENUM(BlueprintType)
namespace EVSTreeQueryMatchType
{
	enum Type
	{
		None			UMETA(Hidden),
		Param,
		Expression
	};
}


/**
 * Leaf query parameters used to evaluate gameplay tag events and gameplay tag state.
 */
USTRUCT(BlueprintType)
struct FVSGameplayTagEventQueryParams
{
	GENERATED_BODY()

	FVSGameplayTagEventQueryParams()
		: EventMatchType(EGameplayContainerMatchType::Any)
		, bMatchExactTagEvents(true)
		, bTriggerEventsEmptyAsPass(false)
		, bPassedInEventsEmptyAsPass(false)
		, bTagQueryEmptyAsPass(false)
	{
	}

	VSPLUGINSCORE_API bool Matches(
		const FGameplayTagContainer& InTagEvents,
		const FGameplayTagContainer& InGameplayTags) const;

	/** Optional editor-facing note describing this params entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** Tag events used as trigger conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagEvents;

	/** Gameplay tag state query evaluated together with TagEvents. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery TagQuery;
	
	/** Comparison mode used when matching configured TagEvents. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayContainerMatchType EventMatchType;
	
	/** If true, only exact tag matches are considered valid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bMatchExactTagEvents : 1;

	/** Treats empty trigger tag set as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTriggerEventsEmptyAsPass : 1;

	/** Treats empty incoming tag events as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bPassedInEventsEmptyAsPass : 1;

	/** Treats empty tag query as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTagQueryEmptyAsPass : 1;
};

/**
 * Recursive expression node combining gameplay tag event query parameters or sub-expressions.
 */
USTRUCT(BlueprintType)
struct FVSGameplayTagEventQueryExpression
{
	GENERATED_BODY()

	bool Matches(
		const FGameplayTagContainer& TagEvents = FGameplayTagContainer(),
		const FGameplayTagContainer& GameplayTags = FGameplayTagContainer()) const;

	/** Optional editor-facing note describing this expression node. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** How child entries are combined (Any / All). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSTreeQueryMatchRange::Type> Range = EVSTreeQueryMatchRange::None;

	/** Selects whether this node evaluates Params or nested Expressions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSTreeQueryMatchType::Type> Type = EVSTreeQueryMatchType::None;

	/** Leaf parameters used when Type == Param. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSTreeQueryMatchType::Param", EditConditionHides))
	TArray<FVSGameplayTagEventQueryParams> Params;

	/** Child expression nodes used when Type == Expression. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSTreeQueryMatchType::Expression", EditConditionHides))
	TArray<TInstancedStruct<FVSGameplayTagEventQueryExpression>> Expressions;
};

/**
 * Root gameplay tag event query evaluated against incoming tag events and gameplay tag state.
 */
USTRUCT(BlueprintType)
struct FVSGameplayTagEventQuery
{
	GENERATED_BODY()

	FVSGameplayTagEventQuery() {}

	VSPLUGINSCORE_API static FVSGameplayTagEventQuery GetEmptyPass();
	VSPLUGINSCORE_API bool Matches(
		const FGameplayTagContainer& TagEvents = FGameplayTagContainer(),
		const FGameplayTagContainer& GameplayTags = FGameplayTagContainer()) const;

	/** Root expression evaluated by this query. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagEventQueryExpression RootExpression;

public:
	static FVSGameplayTagEventQuery Empty;
	static FVSGameplayTagEventQuery EmptyPass;
};


/**
 * Leaf query parameters used to evaluate a scene component against type, class, and tag constraints.
 */
USTRUCT(BlueprintType)
struct FVSSceneComponentQueryParams
{
	GENERATED_BODY()

	FVSSceneComponentQueryParams()
		: bInverseObjectTypes(false)
		, bInverseClassAllowance(false)
		, bInverseComponentTagAllowance(false)
		, bInverseActorTagAllowance(false)
		, bComponentClassesEmptyAsPass(true)
		, bObjectTypesEmptyAsPass(true)
		, bComponentTagsEmptyAsPass(true)
		, bActorTagsEmptyAsPass(true)
	{
	}

	VSPLUGINSCORE_API bool Matches(const USceneComponent* Component) const;

	/** Optional editor-facing note describing this params entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** Allowed object types for the component's owning actor/collision object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	/** Allowed scene component classes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<USceneComponent>> ComponentClasses;

	/** Required component tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ComponentTags;

	/** Required owner-actor tags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ActorTags;

	/** Inverts object type matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bInverseObjectTypes : 1;

	/** Inverts component class matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bInverseClassAllowance : 1;

	/** Inverts component tag matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bInverseComponentTagAllowance : 1;

	/** Inverts actor tag matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bInverseActorTagAllowance : 1;

	/** Treats empty component class list as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bComponentClassesEmptyAsPass : 1;

	/** Treats empty object type list as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bObjectTypesEmptyAsPass : 1;

	/** Treats empty component tag list as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bComponentTagsEmptyAsPass : 1;

	/** Treats empty actor tag list as a successful match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bActorTagsEmptyAsPass : 1;
};

/**
 * Recursive expression node combining scene component query parameters or sub-expressions.
 */
USTRUCT(BlueprintType)
struct FVSSceneComponentQueryExpression
{
	GENERATED_BODY()

	bool Matches(const USceneComponent* Component) const;

	/** Optional editor-facing note describing this expression node. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	/** How child entries are combined (Any / All). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSTreeQueryMatchRange::Type> Range = EVSTreeQueryMatchRange::None;

	/** Selects whether this node evaluates Params or nested Expressions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSTreeQueryMatchType::Type> Type = EVSTreeQueryMatchType::None;

	/** Leaf parameters used when Type == Param. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSTreeQueryMatchType::Param", EditConditionHides))
	TArray<FVSSceneComponentQueryParams> Params;

	/** Child expression nodes used when Type == Expression. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSTreeQueryMatchType::Expression", EditConditionHides))
	TArray<TInstancedStruct<FVSSceneComponentQueryExpression>> Expressions;
};

/**
 * Root scene component query evaluated against a target scene component.
 */
USTRUCT(BlueprintType)
struct FVSSceneComponentQuery
{
	GENERATED_BODY()

	FVSSceneComponentQuery() {}

	VSPLUGINSCORE_API bool Matches(const USceneComponent* Component) const;

	/** Root expression evaluated by this query. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSSceneComponentQueryExpression RootExpression;
};
