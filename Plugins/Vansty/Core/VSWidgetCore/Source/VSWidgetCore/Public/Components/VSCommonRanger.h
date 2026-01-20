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

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FValueChangedSignature, UVSCommonRanger* /** Ranger */, float /** Value */, bool /** bIsMutedValue */);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FMuteStateChangedSignature, UVSCommonRanger* /** Ranger */, bool /** bMuted */);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FValueChangedEvent, UVSCommonRanger*, Ranger, float, Value, bool, bIsMutedValue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMuteStateChangedEvent, UVSCommonRanger*, Ranger, bool, bMuted);

public:
	//~ Begin UCommonButtonBase Interface
	virtual void NativePreConstruct() override;
	virtual bool Initialize() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;

	virtual void NativeOnClicked() override;
	//~ End UCommonButtonBase Interface


	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void SetValue(float NewValue, bool bIsMutedValue = false);

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	float GetValue(bool bIgnoreMuteState = false) const;
	
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	bool IsMuted() const { return bIsMuted; }

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void SetIsMuted(bool bMuted);
	
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	FText GetContentText(float Value, bool bMuted = false, bool bSameValueMutedText = true) const;

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void RefreshRanger();
	
private:
	void OnValueChangedInternal(float Value, bool bIsMutedValue);
	void RefreshContentText();
	
	UFUNCTION()
	void OnWidgetValueChanged(float Value);

	UFUNCTION()
	void OnWidgetMuteStateChanged(bool bMuted);
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USlider> Slider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> SpinBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> CheckBox_Mute;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Content;

public:
	FValueChangedSignature OnValueChanged_Native;
	FMuteStateChangedSignature OnMuteStateChanged_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FValueChangedEvent OnValueChanged;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FMuteStateChangedEvent OnMuteStateChanged;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FVector2D ValueRange = FVector2D(0.0, 1.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float StepSize = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bSnapByStep : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bSupportMutation : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float MutedStateValue = 0.f;

	/**
	 * If false, a refreshment of widgets will be done during pre-construction.
	 * If true, widgets refreshment should be executed manually.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bDifferRefreshment : 1;
	
	/** The content text format to display. If you want to show the digits, please put ‘{0}’ in it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayFormatText = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	/** The text to display when muted. Not work if left empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayMutedText = FText::FromString("");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bDisplaySameValueMutedText : 1;

private:
#if WITH_EDITORONLY_DATA
	/** Non muted. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Value", Category = "Ranger")
	float EditorPreviewValue = 0.f;

	UPROPERTY(EditAnywhere, DisplayName = "Preview Mute State", Category = "Ranger")
	uint8 EditorPreviewMuteState : 1;
#endif

	/** The value that is set and is not concerned with mutation. Sync with slider and spinbox. */
	float CurrentNonMuteValue = 0.f;
	float LastRefreshedMutedValue = 0.f;
	uint8 bIsMuted : 1;
	uint8 bValueDelegatesBound : 1;
	uint8 bMuteDelegateBound : 1;
};
