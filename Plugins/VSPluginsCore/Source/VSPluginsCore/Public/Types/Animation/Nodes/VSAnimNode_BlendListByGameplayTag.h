// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_BlendListBase.h"
#include "VSAnimNode_BlendListByGameplayTag.generated.h"

USTRUCT(BlueprintType)
struct VSPLUGINSCORE_API FVSAnimNode_BlendListByGameplayTag : public FAnimNode_BlendListBase
{
	GENERATED_BODY()
	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (FoldProperty, PinShownByDefault))
	FGameplayTag ActiveTag;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (FoldProperty))
	TArray<FGameplayTag> Tags;
#endif

protected:
	virtual int32 GetActiveChildIndex() override;

public:
	const FGameplayTag& GetActiveTag() const;
	const TArray<FGameplayTag>& GetTags() const;

#if WITH_EDITOR
	void RefreshPosePins();
#endif
};
