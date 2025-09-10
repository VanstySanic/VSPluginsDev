// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "VSComboBoxString.generated.h"

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSComboBoxString : public UComboBoxString
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ComboBox")
	void SetTextJustification(ETextJustify::Type InJustification);

protected:
	virtual TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> Item) const override;


	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
	TEnumAsByte<ETextJustify::Type> TextJustification = ETextJustify::Center;
};
