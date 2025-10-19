// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSQueryMatchTypes.h"
#include "UObject/Object.h"
#include "VSGameplayTagQueryExpression.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, DefaultToInstanced, EditInlineNew)
class VSPLUGINSCORE_API UVSGameplayTagQueryExpression : public UObject
{
	GENERATED_UCLASS_BODY()
	
	bool Matches(const FGameplayTag& TagEvent = FGameplayTag(), const FGameplayTagContainer& GameplayTags = FGameplayTagContainer()) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchRange::Type> Range = EVSQueryMatchRange::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchType::Type> Type = EVSQueryMatchType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Params", EditConditionHides))
	TArray<FVSGameplayTagEventQueryParams> Params;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta = (EditCondition = "Type == EVSQueryMatchType::Expression", EditConditionHides))
	TArray<TObjectPtr<UVSGameplayTagQueryExpression>> Expressions;
};
