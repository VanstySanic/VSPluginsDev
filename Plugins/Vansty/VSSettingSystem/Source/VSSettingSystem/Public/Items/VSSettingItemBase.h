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
	void NotifyUpdate();

public:
	UFUNCTION(BlueprintCallable, Category = "Feature")
	FVSSettingItemInfo GetItemInfo() const { return ItemInfo; }

	/**
	 * Bind a widget of specified type id to the setting item. A widget can only be bound once.
	 * @param TypeID Widget type.
	 *		It could be:
	 *		Item (The composed widget that handles the item),
	 *		Name (The widget is normally TextBlock), Content (The widget is normally TextBlock or RichTextBlock, etc.),
	 *		Core (The widget is normally checkbox, combobox or rotator, etc.)
	 *		or any custom type id that you'll need to handle manually.
	 */
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void BindWidget(UWidget* Widget, FName TypeID = NAME_None);
	
	UFUNCTION(BlueprintCallable, Category = "Feature")
	void UnbindWidget(UWidget* Widget, FName TypeID);

protected:
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
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDirty() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsDefault() const;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool IsUnconfirmed() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	void SetToBySource(const EVSSettingItemValueSource::Type ValueSource);

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EqualsToBySource(const EVSSettingItemValueSource::Type ValueSource) const;

	
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemUpdateSignature OnUpdated;
	
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FSettingItemActionSignature OnActionExecuted;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FVSSettingItemInfo ItemInfo;

private:
	bool bHasBeenInitialized = false;
};
