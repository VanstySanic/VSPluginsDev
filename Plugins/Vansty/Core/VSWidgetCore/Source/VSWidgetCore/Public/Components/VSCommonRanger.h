// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "VSCommonRanger.generated.h"

class UCheckBox;
class UTextBlock;
class USpinBox;
class USlider;

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonRanger : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FValueChangedSignature, UVSCommonRanger* /** Ranger */, float /** Value */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FValueChangedEvent, UVSCommonRanger*, Ranger, float, Value);

public:
	//~ Begin UCommonButtonBase Interface
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual bool Initialize() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//~ End UCommonButtonBase Interface
	
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void SetValue(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual float GetValue() const { return CurrentValue; }
	
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual FText GetContentText() const;

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual void RefreshRanger();

protected:
	virtual void OnValueChangedInternal();
	void RefreshContentText();

	UFUNCTION()
	virtual void OnWidgetValueChanged(float Value);

private:
	void OnCultureChanged();
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USlider> Slider;

	/** Auxiliary value and display. */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> SpinBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Content;

public:
	FValueChangedSignature OnValueChanged_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FValueChangedEvent OnValueChanged;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FVector2D ValueRange = FVector2D(0.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float StepSize = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bSnapByStep : 1;

	/**
	 * If false, a refreshment of widgets will be done during pre-construction.
	 * If true, widgets refreshment should be executed manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bDifferRefreshment : 1;
	
	/** The content text format to display. If you want to show the digits, please put ‘{0}’ in it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FIntPoint DisplayFractionDigitRange = FIntPoint(0, 324);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float DisplayValueMultiplier = 1.f;

private:
#if WITH_EDITORONLY_DATA
	/** Non muted. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Value", Category = "Ranger")
	float EditorPreviewValue = 0.f;
#endif

	/** The value that is set and is not concerned with mutation. Sync with slider and spinbox. */
	float CurrentValue = 0.f;
	uint8 bValueDelegatesBound : 1;
};
