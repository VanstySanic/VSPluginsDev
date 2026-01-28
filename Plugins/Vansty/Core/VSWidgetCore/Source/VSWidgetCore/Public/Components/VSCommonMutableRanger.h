// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCommonRanger.h"
#include "VSCommonMutableRanger.generated.h"

/**
 * 
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonMutableRanger : public UVSCommonRanger
{
	GENERATED_UCLASS_BODY()
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FMuteStateChangedSignature, UVSCommonMutableRanger* /** Ranger */, bool /** bMuted */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMuteStateChangedEvent, UVSCommonRanger*, Ranger, bool, bMuted);

public:
	//~ Begin UVSCommonRanger Interface.
	virtual void NativePreConstruct() override;
	virtual bool Initialize() override;
	//~ End UVSCommonRanger Interface.

	virtual float GetValue() const override;
	virtual void RefreshRanger() override;
	virtual FText GetContentText() const override;

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	bool GetIsMuted() const { return bIsMuted; }

	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void SetIsMuted(bool bMuted);
	
protected:
	virtual void OnWidgetValueChanged(float Value) override;
	virtual void OnValueChangedInternal() override;
	virtual void OnMuteStateChangedInternal();

private:
	UFUNCTION()
	void OnWidgetMuteStateChanged(bool bMuted);
	
public:
	FValueChangedSignature OnNonMutedValueChanged_Native;
	FMuteStateChangedSignature OnMuteStateChanged_Native;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FValueChangedEvent OnNonMutedValueChanged;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FMuteStateChangedEvent OnMuteStateChanged;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float MuteStateValue = 0.f;
	
	/** The text to display when muted. Not work if left empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayMutedText = FText::FromString("");

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> CheckBox_Mute;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Mute State", Category = "Ranger")
	uint8 EditorPreviewMuteState : 1;

	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Value with Mute State", Category = "Ranger")
	uint8 EditorPreviewValueWithMuteState : 1;
#endif
	
	uint8 bIsMuted : 1;
	float LastRefreshedMutedValue = 0.f;
	uint8 bMuteDelegateBound : 1;
};
