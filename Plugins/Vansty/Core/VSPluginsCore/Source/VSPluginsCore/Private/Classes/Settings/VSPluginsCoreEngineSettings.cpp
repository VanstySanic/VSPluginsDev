// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Settings/VSPluginsCoreEngineSettings.h"
#include "Classes/Libraries/VSPlatformLibrary.h"

UVSPluginsCoreEngineSettings::UVSPluginsCoreEngineSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SectionName = FName("VS Plugins Core Engine Settings");
}

void UVSPluginsCoreEngineSettings::PostInitProperties()
{
	Super::PostInitProperties();

	CVarDesiredFullscreenMonitorID->OnChangedDelegate().AddUObject(this, &UVSPluginsCoreEngineSettings::OnCVarDesiredFullscreenMonitorIDChanged);
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &UVSPluginsCoreEngineSettings::OnApplicationActivationStateChanged);
	}
}

FName UVSPluginsCoreEngineSettings::GetCategoryName() const
{
	return FName("Engine");
}

const UVSPluginsCoreEngineSettings* UVSPluginsCoreEngineSettings::Get()
{
	return GetDefault<UVSPluginsCoreEngineSettings>();
}

const UVSPluginsCoreEngineSettings* UVSPluginsCoreEngineSettings::GetVSPluginsCoreEngineSettings()
{
	return Get();
}


void UVSPluginsCoreEngineSettings::CheckFullscreenDesiredMonitor()
{
	FIntPoint ScreenResolution = FIntPoint(GSystemResolution.ResX, GSystemResolution.ResY);
	TEnumAsByte<EWindowMode::Type> WindowMode = GSystemResolution.WindowMode;
	if (IConsoleVariable* CVarSetRes = IConsoleManager::Get().FindConsoleVariable(TEXT("r.SetRes"), false))
	{
		UVSPlatformLibrary::ParseSetRes(CVarSetRes->GetString(), ScreenResolution, WindowMode);
	}
	if (WindowMode == EWindowMode::Fullscreen)
	{
		FVSMonitorInfo MonitorInfo = UVSPlatformLibrary::GetMonitorInfoByID(CVarDesiredFullscreenMonitorID->GetString());
		if (MonitorInfo.ID.IsEmpty())
		{
			MonitorInfo = UVSPlatformLibrary::GetPrimaryMonitorInfo();
		}

		const FVector2D& WindowRootPos = UVSPlatformLibrary::GetWindowRootPosition(false);
		const FString& WindowMonitorID = UVSPlatformLibrary::GetMonitorIDByPosition(WindowRootPos);
		const FMonitorInfo& WindowMonitorInfo = UVSPlatformLibrary::GetNativeMonitorInfoByID(WindowMonitorID);
		if (!MonitorInfo.ID.IsEmpty() && WindowMonitorInfo.ID != MonitorInfo.ID)
		{
			UVSPlatformLibrary::SwitchMonitorByID(MonitorInfo.ID);
		}
	}
}


void UVSPluginsCoreEngineSettings::OnCVarDesiredFullscreenMonitorIDChanged(IConsoleVariable* ConsoleVariable)
{
	CheckFullscreenDesiredMonitor();
}

void UVSPluginsCoreEngineSettings::OnApplicationActivationStateChanged(bool bIsApplicationActive)
{
	CheckFullscreenDesiredMonitor();
}
