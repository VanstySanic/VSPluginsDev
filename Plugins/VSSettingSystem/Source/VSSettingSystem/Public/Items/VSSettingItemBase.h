// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/VSSettingSystemTypes.h"
#include "UObject/Object.h"
#include "VSSettingItemBase.generated.h"

class UWidget;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew, DisplayName = "Settings.Item.Base")
class VSSETTINGSYSTEM_API UVSSettingItemBase : public UObject
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingItemUpdateSignature);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingItemActionSignature, EVSSettingItemAction::Type, Action);

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void Uninitialize();

	UFUNCTION(BlueprintCallable, Category = "Feature")
	bool HasBeenInitialized() const { return bHasBeenInitialized; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ExecuteAction(TEnumAsByte<EVSSettingItemAction::Type> Action);

	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "Actions"))
	void ExecuteActions(const TArray<TEnumAsByte<EVSSettingItemAction::Type>>& Actions);
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifyValueUpdate();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDefault() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FVSSettingItemInfo GetItemInfo() const { return ItemInfo; }

	/**
	 * Bind a widget of specified type id to the setting item. A widget can only be bound once.
	 * @param TypeID Widget type.
	 *		It could be:
	 *		Item (The composed widget that handles the item),
	 *		Name (The widget is normally TextBlock), Content (The widget is normally TextBlock or RichTextBlock, etc.),
	 *		Value (The widget is normally CheckBox, ComboBox or Slider, etc.)
	 *		or any custom type id that you'll need to handle manually.
	 */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void BindWidget(UWidget* Widget, FName TypeID = NAME_None);

	/** Unbind the widget, then bind the widget again. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void RebindWidget(UWidget* Widget, FName TypeID = NAME_None);
	
	/** This is possibly unused. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void UnbindWidget(UWidget* Widget, FName TypeID);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	bool IsWidgetBound(UWidget* Widget) const;

	UFUNCTION(BlueprintCallable, Category = "Widget")
	FName GetWidgetBoundTypeID(UWidget* Widget) const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Widget")
	TArray<UWidget*> GetBoundWidgetsOfType(FName TypeName) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Load();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Validate();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Apply();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Confirm();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void Save();

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void SetToBySource(const EVSSettingItemValueSource::Type ValueSource);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EqualsToBySource(const EVSSettingItemValueSource::Type ValueSource) const;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void OnItemValueUpdated();

	/**
	 * Implement this cause some widget bindings may have concern with culture and language.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Feature")
	void OnCultureChanged();
	
	/**
	 * Bind a widget of specified type id to the setting item. A widget can only be bound once.
	 * @param TypeName Widget type.
	 *		It could be:
	 *		Item (The composed widget that handles the item),
	 *		Name (The widget is normally TextBlock),
	 *		Value (The widget is normally CheckBox, ComboBox or Slider, etc.),
	 *		Content (The widget is normally TextBlock or RichTextBlock, etc.),
	 *		or any custom type id that you'll need to handle manually.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void BindWidgetInternal(UWidget* Widget, FName TypeName = NAME_None);

	/** Unbind widget from the setting item. Normally needs to be called when rebinding or when the widget is destroyed. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void UnbindWidgetInternal(UWidget* Widget, FName TypeName);

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateSignature OnValueUpdated;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemActionSignature OnActionExecuted;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemInfo ItemInfo;

	/** Executed when the value is updated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<TEnumAsByte<EVSSettingItemAction::Type>> ValueUpdatedActions;

private:
	bool bHasBeenInitialized = false;
	TMultiMap<TWeakObjectPtr<UWidget>, FName> TypedBoundWidgetMap;
	TMultiMap<FName, TWeakObjectPtr<UWidget>> TypedBoundWidgetMultimap;
	FDelegateHandle OnCultureChangedDelegateHandle;
	FDelegateHandle OnWorldBeginTearDownDelegateHandle;
};
