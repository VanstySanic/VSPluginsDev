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
	if (GSystemResolution.WindowMode == EWindowMode::Fullscreen)
	{
		FVSMonitorInfo MonitorInfo = UVSPlatformLibrary::GetMonitorInfoByID(CVarDesiredFullscreenMonitorID->GetString());
		if (MonitorInfo.ID.IsEmpty())
		{
			MonitorInfo = UVSPlatformLibrary::GetPrimaryMonitorInfo();
		}
		if (!MonitorInfo.ID.IsEmpty())
		{
			UVSPlatformLibrary::SwitchMonitorByID(CVarDesiredFullscreenMonitorID->GetString());
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
