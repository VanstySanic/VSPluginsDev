// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCommonRanger.h"
#include "VSCommonMutableRanger.generated.h"

/**
 * Ranger widget with a dedicated mute state.
 *
 * When muted, exposed value becomes MuteStateValue and display text can be
 * replaced by DisplayMutedText.
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

	/** Returns current mute state. */
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	bool GetIsMuted() const { return bIsMuted; }

	/** Sets mute state. May stay muted when current non-muted value equals `MuteStateValue`. */
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
	/** Broadcast when non-muted value changes. */
	FValueChangedSignature OnNonMutedValueChanged_Native;
	/** Broadcast when mute state changes. */
	FMuteStateChangedSignature OnMuteStateChanged_Native;

	/** Broadcast when non-muted value changes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FValueChangedEvent OnNonMutedValueChanged;

	/** Broadcast when mute state changes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FMuteStateChangedEvent OnMuteStateChanged;
	
public:
	/** Value exposed when muted. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float MuteStateValue = 0.f;
	
	/** Text used when muted. Ignored when empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayMutedText = FText::FromString("");

protected:
	/** Optional mute checkbox bound to `bIsMuted`. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> CheckBox_Mute;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Mute State", Category = "Ranger")
	uint8 EditorPreviewMuteState : 1;

	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Value with Mute State", Category = "Ranger")
	uint8 EditorPreviewValueWithMuteState : 1;
#endif
	
	uint8 bIsMuted : 1;
	/** Last refreshed mute value used to detect editor/runtime display refresh needs. */
	float LastRefreshedMutedValue = 0.f;
	/** Tracks whether checkbox delegate is currently bound. */
	uint8 bMuteDelegateBound : 1;
};
