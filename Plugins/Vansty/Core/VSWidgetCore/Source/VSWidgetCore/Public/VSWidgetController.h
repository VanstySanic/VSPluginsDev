// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/WidgetChild.h"
#include "Classes/VSTickableObject.h"
#include "VSWidgetController.generated.h"

class UVSWidgetBinder;
class UWidget;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class VSWIDGETCORE_API UVSWidgetController : public UVSTickableObject
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

	UFUNCTION(BlueprintCallable, Category = "Widget Controller", meta = (AutoCreateRefTerm = "TypedWidgetsToBind"))
	void Register(const TMap<FName, UWidget*>& TypedWidgetsToBind);

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void Unregister();

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void Reregister();
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	bool IsRegistered() { return bIsRegistered; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	bool HasBeenInitialized() { return bHasBeenInitialized; }
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void BindWidget(UWidget* Widget, const FName TypeName);
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void UnbindWidget(UWidget* Widget, const FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	void RebindWidgetByType(const FName TypeName);
	
	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	UWidget* GetBoundTypedWidget(const FName TypeName);

	UFUNCTION(BlueprintCallable, Category = "Widget Controller")
	TMap<FName, UWidget*> GetAllBoundTypedWidgets() const;
	
protected:
	//~ Begin UVSTickableObject Interface
	virtual void TickObject(float DeltaTime, ELevelTick TickType, FVSObjectTickFunction* TickFunction) override;
	//~ End UVSTickableObject Interface

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void Uninitialize();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void BindTypedWidget(const FName TypeName, UWidget* Widget);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Widget Controller")
	void UnbindTypedWidget(const FName TypeName, UWidget* Widget);


protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "Widget Controller")
	TArray<TObjectPtr<UVSWidgetBinder>> WidgetBinders;

private:
	uint8 bIsRegistered : 1;
	uint8 bHasBeenInitialized : 1;

	TMap<FName, TWeakObjectPtr<UWidget>> BoundTypedWidgets;
};
