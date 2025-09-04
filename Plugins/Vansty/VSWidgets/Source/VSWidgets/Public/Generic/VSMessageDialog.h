// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Types/VSWidgetTypes.h"
#include "VSMessageDialog.generated.h"

class UCommonButtonBase;
struct FVSWidgetMessageDialogParams;
class UTextBlock;
class URichTextBlock;

/**
 * 
 */
UCLASS()
class VSWIDGETS_API UVSMessageDialog : public UCommonActivatableWidget
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageDialogReplySignature, FName, Reply);

public:
	UFUNCTION(BlueprintCallable, Category = "Widget", meta = (AutoCreateRefTerm = "Params"))
	static UVSMessageDialog* CreateMessageDialog(APlayerController* PlayerController, const TSubclassOf<UVSMessageDialog> Class, const FVSWidgetMessageDialogParams& Params, bool bAutoDisplay = true);

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	void GenerateAndInitializeButtons();
	void OnDialogReplied();

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Title;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> Panel_Button;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URichTextBlock> RichTextBlock_Message;
	
	/** This button is used as a sample for size and paddings. This is optional. */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonButtonBase> Button_Sample;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TMap<FName, TSubclassOf<UCommonButtonBase>> RepliedButtonClasses;

	UPROPERTY(EditAnywhere, Category = "Widget", meta = (RowType = "/Script/VSWidgets.VSCommonButtonDisplayParams"))
	TMap<FName, FDataTableRowHandle> RepliedButtonDisplayParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	bool bRestoreActiveUIInputConfig = true;
	
public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FMessageDialogReplySignature OnReplied;

private:
	UPROPERTY()
	TMap<TObjectPtr<UCommonButtonBase>, FName> NamedReplyGeneratedButtons;

	FVSWidgetMessageDialogParams MessageDialogParams;
	FUIInputConfig CachedUIInputConfig;
};
