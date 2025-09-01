// Copyright VanstySanic. All Rights Reserved.

#include "Items/ConsoleVariable/VSSettingItem_ConsoleVariableBase.h"

#include "Kismet/KismetSystemLibrary.h"

UVSSettingItem_ConsoleVariableBase::UVSSettingItem_ConsoleVariableBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ConsoleVariableBase::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	ConsoleVariable = IConsoleManager::Get().FindConsoleVariable(*ConsoleVariableName);
	check(ConsoleVariable);

	OnVariableChangedDelegateHandle = ConsoleVariable->OnChangedDelegate().AddUObject(this, &UVSSettingItem_ConsoleVariableBase::OnConsoleVariableChanged);
}

void UVSSettingItem_ConsoleVariableBase::Uninitialize_Implementation()
{
	ConsoleVariable->OnChangedDelegate().Remove(OnVariableChangedDelegateHandle);
	
	Super::Uninitialize_Implementation();
}


void UVSSettingItem_ConsoleVariableBase::OnConsoleVariableChanged(IConsoleVariable* Variable)
{
	
}
