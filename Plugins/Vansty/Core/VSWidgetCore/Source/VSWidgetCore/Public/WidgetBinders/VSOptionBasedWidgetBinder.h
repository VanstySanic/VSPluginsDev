// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSOptionBasedWidgetBinder.generated.h"

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSOptionBasedWidgetBinder : public UVSWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetBinder Interface
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option")
	FString GetCurrentOptionKey() const;
	
	UFUNCTION(BlueprintCallable, Category = "Option")
	void RefreshOptions();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option")
	TMap<FString, FText> GetOptionKeyedTexts() const;

	UFUNCTION(BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Text"))
	int32 GetSelectedIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Option")
	FText GetCurrentOptionText() const;
	
	UFUNCTION(BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "String"))
	FText GetOptionTextByKey(const FString& String) const;
	
	UFUNCTION(BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Text"))
	FString GetOptionKeyByText(const FText& Text) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Option")
	void OnWidgetOptionChanged(const FString& NewKey, const FText& NewText, int32 NewIndex);

private:
	void OnCultureChanged();

	UPROPERTY(VisibleAnywhere, Category = "Option")
	TMap<FString, FText> OptionKeyedTexts;
};
