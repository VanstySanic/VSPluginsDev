// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_AntiAliasingMethod.h"
#include "VSSettingSystemConfig.h"
#include "Engine/RendererSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_AntiAliasingMethod::UVSSettingItem_AntiAliasingMethod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemIdentifier = EVSSettingItem::Graphics::AntiAliasingMethod;
	ItemInfo.ItemTags.AddTag(EVSSettingItem::Graphics::AntiAliasingMethod.GetTag().RequestDirectParent());
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Graphics.AntiAliasingMethod", "DisplayName", "Anti-Aliasing Method");
	ConfigSettings.Section = "VS.SettingSystem.Item.Graphics";
	ConfigSettings.PrimaryKey = "r.AntiAliasingMethod";

	SetConsoleVariableName("r.AntiAliasingMethod");
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
	const EAntiAliasingMethod NewAntiAliasingMethod = (EAntiAliasingMethod)FMath::Clamp(PrevAntiAliasingMethod, 0, AAM_MAX - 1);
	if (PrevAntiAliasingMethod != NewAntiAliasingMethod)
	{
		SetIntegerValue(NewAntiAliasingMethod);
	}
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

FText UVSSettingItem_AntiAliasingMethod::ValueStringToText_Implementation(const FString& String) const
{
	static TMap<FString, FText> OutMap = TMap<FString, FText>
	{
		{ "0", UVSSettingSystemConfig::Get()->AntiAliasingMethodDisplayNames.FindRef(AAM_None) },
		{ "1", UVSSettingSystemConfig::Get()->AntiAliasingMethodDisplayNames.FindRef(AAM_FXAA) },
		{ "2", UVSSettingSystemConfig::Get()->AntiAliasingMethodDisplayNames.FindRef(AAM_TemporalAA) },
		{ "3", UVSSettingSystemConfig::Get()->AntiAliasingMethodDisplayNames.FindRef(AAM_MSAA) },
		{ "4", UVSSettingSystemConfig::Get()->AntiAliasingMethodDisplayNames.FindRef(AAM_TSR) },
	};
	
	return OutMap.FindRef(String);
}

void UVSSettingItem_AntiAliasingMethod::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();
	
#if WITH_EDITOR
	EditorPreviewAntiAliasingMethod = GetAntiAliasingMethod(EVSSettingItemValueSource::System);
#endif
}

void UVSSettingItem_AntiAliasingMethod::SetAntiAliasingMethod(EAntiAliasingMethod InMethod)
{
	SetIntegerValue(static_cast<int32>(InMethod));
}

EAntiAliasingMethod UVSSettingItem_AntiAliasingMethod::GetAntiAliasingMethod(EVSSettingItemValueSource::Type ValueSource) const
{
	return static_cast<EAntiAliasingMethod>(GetIntegerValue(ValueSource));
}
