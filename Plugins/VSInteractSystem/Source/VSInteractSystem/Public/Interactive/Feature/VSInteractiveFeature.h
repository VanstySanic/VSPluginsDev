// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Framework/VSObjectFeature.h"
#include "VSInteractiveFeature.generated.h"

class UVSInteractFeatureAgent;
class UVSInteractiveFeatureAgent;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Feature.Interactive.Base")
class VSINTERACTSYSTEM_API UVSInteractiveFeature : public UVSObjectFeature
{
	GENERATED_UCLASS_BODY()
	friend class UVSInteractiveFeatureAgent;

public:
	virtual void Initialize_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	UVSInteractiveFeatureAgent* GetInteractiveFeatureAgent() const { return InteractiveFeatureAgentPrivate.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	bool IsInspectedBySource(UVSInteractFeatureAgent* SourceAgent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Interactive")
	bool IsInteractedBySource(UVSInteractFeatureAgent* SourceAgent) const;

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	float GetInteractionTime(UVSInteractFeatureAgent* SourceAgent) const;
	
private:
	TWeakObjectPtr<UVSInteractiveFeatureAgent> InteractiveFeatureAgentPrivate;
};
