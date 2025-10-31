// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "VSQueryMatchTypes.generated.h"

class UVSSceneComponentQueryExpression;
class UVSGameplayTagQueryExpression;

UENUM(BlueprintType)
namespace EVSQueryMatchRange
{
	enum Type
	{
		None,
		Any,
		All,
	};
}

UENUM(BlueprintType)
namespace EVSQueryMatchType
{
	enum Type
	{
		None				UMETA(Hidden),
		Params,
		Expression
	};
}

USTRUCT(BlueprintType)
struct FVSGameplayTagEventQueryParams
{
	GENERATED_BODY()

	FVSGameplayTagEventQueryParams()
		: bMatchExactTagEvent(true)
		, bEmptyEventAsPass(false)
		, bTagEventsEmptyAsPass(false)
		, bTagQueryEmptyAsPass(false)
	{
	}

	VSPLUGINSCORE_API bool Matches(const FGameplayTag& TagEvent, const FGameplayTagContainer& GameplayTags) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	/** Work as an entry. Requires TagEvent in the array. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagEvents;

	/** Gameplay tag query to check. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery TagQuery;

	/** If true, only exact tag event will match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bMatchExactTagEvent : 1;
	
	/** If true, empty tag will be considered as a pass to the tag events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bEmptyEventAsPass : 1;
	
	/** If true and TagEvents array is empty, any non-empty tag event will be considered as trigger. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTagEventsEmptyAsPass : 1;

	/** If true and TagQuery is empty, any tag states will match the query. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTagQueryEmptyAsPass : 1;
};

USTRUCT(BlueprintType)
struct FVSGameplayTagQueryExpression
{
	GENERATED_BODY()
	
	bool Matches(const FGameplayTag& TagEvent = FGameplayTag(), const FGameplayTagContainer& GameplayTags = FGameplayTagContainer()) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchRange::Type> Range = EVSQueryMatchRange::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchType::Type> Type = EVSQueryMatchType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Params", EditConditionHides))
	TArray<FVSGameplayTagEventQueryParams> Params;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Expression", EditConditionHides))
	TArray<TInstancedStruct<FVSGameplayTagQueryExpression>> Expressions;
};

USTRUCT(BlueprintType)
struct FVSGameplayTagEventQuery
{
	GENERATED_BODY()
	
	FVSGameplayTagEventQuery() {}
	
	VSPLUGINSCORE_API static FVSGameplayTagEventQuery GetEmptyPass();
	VSPLUGINSCORE_API bool Matches(const FGameplayTag& TagEvent = FGameplayTag(), const FGameplayTagContainer& GameplayTags = FGameplayTagContainer()) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSGameplayTagQueryExpression RootExpression;

public:
	static FVSGameplayTagEventQuery Empty;
	static FVSGameplayTagEventQuery EmptyPass;
};


USTRUCT(BlueprintType)
struct FVSSceneComponentQueryParams
{
	GENERATED_BODY()

	VSPLUGINSCORE_API bool Matches(const USceneComponent* Component) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<USceneComponent>> ComponentClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ComponentTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ActorTags;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseObjectTypes = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseClassAllowance = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseComponentTagAllowance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseActorTagAllowance = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComponentClassesEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bObjectTypesEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComponentTagsEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActorTagsEmptyAsPass = true;
};

USTRUCT(BlueprintType)
struct FVSSceneComponentQueryExpression
{
	GENERATED_BODY()

	bool Matches(const USceneComponent* Component) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchRange::Type> Range = EVSQueryMatchRange::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchType::Type> Type = EVSQueryMatchType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Params", EditConditionHides))
	TArray<FVSSceneComponentQueryParams> Params;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Expression", EditConditionHides))
	TArray<TInstancedStruct<FVSSceneComponentQueryExpression>> Expressions;
};

USTRUCT(BlueprintType)
struct FVSSceneComponentQuery
{
	GENERATED_BODY()
	
	FVSSceneComponentQuery() {}

	VSPLUGINSCORE_API bool Matches(const USceneComponent* Component) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVSSceneComponentQueryExpression RootExpression;
};
