// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_AntiAliasingMethod.h"
#include "Engine/RendererSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_AntiAliasingMethod::UVSSettingItem_AntiAliasingMethod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemTag = EVSSettingItem::Graphics::AntiAliasingMethod;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Graphics.AntiAliasingMethod", "DisplayName", "Anti-Aliasing Method");
	ConfigParams.ConfigSection = FString("VS.SettingSystem.Item.Graphics.AntiAliasingMethod");
	ConfigParams.ConfigKeyName = FString("r.AntiAliasingMethod");

	SetConsoleVariableName("r.AntiAliasingMethod");
}

void UVSSettingItem_AntiAliasingMethod::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorPreviewAntiAliasingMethod = GetAntiAliasingMethod(EVSSettingItemValueSource::System);
#endif
}

#if WITH_EDITOR
void UVSSettingItem_AntiAliasingMethod::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingItem_AntiAliasingMethod, EditorPreviewAntiAliasingMethod))
	{
		SetIntegerValue(EditorPreviewAntiAliasingMethod);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

bool UVSSettingItem_AntiAliasingMethod::IsValueValid_Implementation() const
{
	const EAntiAliasingMethod AntiAliasingMethod = GetAntiAliasingMethod(EVSSettingItemValueSource::System);
	return FVSMath::IsInRange(AntiAliasingMethod, 0, AAM_MAX - 1);
}

void UVSSettingItem_AntiAliasingMethod::Validate_Implementation()
{
	Super::Validate_Implementation();

	const EAntiAliasingMethod PrevAntiAliasingMethod = GetAntiAliasingMethod(EVSSettingItemValueSource::System);
	SetIntegerValue(FMath::Clamp(PrevAntiAliasingMethod, 0, AAM_MAX - 1));
}

int32 UVSSettingItem_AntiAliasingMethod::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return Super::GetIntegerValue_Implementation(ValueSource);

	if (ValueType == EVSCommonSettingValueType::Integer)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			if (const URendererSettings* RendererSettings = GetDefault<URendererSettings>())
			{
				return RendererSettings->DefaultFeatureAntiAliasing;
			}

		default: ;
		}
	}
		
	return Super::GetIntegerValue_Implementation(ValueSource);
}

void UVSSettingItem_AntiAliasingMethod::SetAntiAliasingMethod(EAntiAliasingMethod InMethod)
{
	SetIntegerValue(static_cast<int32>(InMethod));
}

EAntiAliasingMethod UVSSettingItem_AntiAliasingMethod::GetAntiAliasingMethod(EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<EAntiAliasingMethod>(GetIntegerValue(ValueSource));
}
