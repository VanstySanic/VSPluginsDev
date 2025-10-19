// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSQueryMatchTypes.h"
#include "UObject/Object.h"
#include "VSSceneComponentQueryExpression.generated.h"

/**
 * 
 */
UCLASS()
class VSPLUGINSCORE_API UVSSceneComponentQueryExpression : public UObject
{
	GENERATED_UCLASS_BODY()

	bool Matches(const USceneComponent* Component) const;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchRange::Type> Range = EVSQueryMatchRange::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSQueryMatchType::Type> Type = EVSQueryMatchType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Params", EditConditionHides))
	TArray<FVSSceneComponentQueryParams> Params;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Type == EVSQueryMatchType::Expression", EditConditionHides))
	TArray<TObjectPtr<UVSSceneComponentQueryExpression>> Expressions;
};
