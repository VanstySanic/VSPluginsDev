// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Settings/VSPluginsCoreEngineSettings.h"
#include "Classes/Libraries/VSPlatformLibrary.h"

TAutoConsoleVariable<FString> CVarDesiredFullscreenMonitorID(
	TEXT("r.DesiredFullscreenMonitorID"),
	"",
	TEXT("Target monitor ID for exclusive fullscreen (fallbacks to primary if empty or invalid)."),
	ECVF_Scalability);

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

void UVSPluginsCoreEngineSettings::BeginDestroy()
{
	CVarDesiredFullscreenMonitorID->OnChangedDelegate().RemoveAll(this);
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().RemoveAll(this);
	}
	
	Super::BeginDestroy();
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
		FString MonitorID = CVarDesiredFullscreenMonitorID->GetString();
		if (MonitorID.IsEmpty() || !UVSPlatformLibrary::IsValidMonitorID(MonitorID))
		{
			MonitorID = UVSPlatformLibrary::GetPrimaryMonitorID();
		}
		
		FVSMonitorInfo MonitorInfo = UVSPlatformLibrary::GetMonitorInfoByID(MonitorID);
		
		const FVector2D WindowRootPos = UVSPlatformLibrary::GetGameWindowRootPosition(false);
		const FString WindowMonitorID = UVSPlatformLibrary::GetMonitorIDByPosition(WindowRootPos);
		const FMonitorInfo WindowMonitorInfo = UVSPlatformLibrary::GetNativeMonitorInfoByID(WindowMonitorID);
		if (!MonitorID.IsEmpty() && WindowMonitorInfo.ID != MonitorInfo.ID)
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
