// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AI_Changing/Public/AI_ChangingSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAI_ChangingSettings() {}

// Begin Cross Module References
AI_CHANGING_API UClass* Z_Construct_UClass_UAI_ChangingSettings();
AI_CHANGING_API UClass* Z_Construct_UClass_UAI_ChangingSettings_NoRegister();
DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings();
UPackage* Z_Construct_UPackage__Script_AI_Changing();
// End Cross Module References

// Begin Class UAI_ChangingSettings
void UAI_ChangingSettings::StaticRegisterNativesUAI_ChangingSettings()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAI_ChangingSettings);
UClass* Z_Construct_UClass_UAI_ChangingSettings_NoRegister()
{
	return UAI_ChangingSettings::StaticClass();
}
struct Z_Construct_UClass_UAI_ChangingSettings_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Settings for the AI_Changing plugin\n */" },
		{ "DisplayName", "AI Chat Settings" },
		{ "IncludePath", "AI_ChangingSettings.h" },
		{ "ModuleRelativePath", "Public/AI_ChangingSettings.h" },
		{ "ToolTip", "Settings for the AI_Changing plugin" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AIServerURL_MetaData[] = {
		{ "Category", "API" },
		{ "Comment", "// The base URL of the AI API server\n" },
		{ "DisplayName", "AI Server URL" },
		{ "ModuleRelativePath", "Public/AI_ChangingSettings.h" },
		{ "ToolTip", "The base URL of the AI API server" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ChatCompletionsEndpoint_MetaData[] = {
		{ "Category", "API" },
		{ "Comment", "// Path to send chat completion requests\n" },
		{ "DisplayName", "Chat Completions Endpoint" },
		{ "ModuleRelativePath", "Public/AI_ChangingSettings.h" },
		{ "ToolTip", "Path to send chat completion requests" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_AIServerURL;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ChatCompletionsEndpoint;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAI_ChangingSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UAI_ChangingSettings_Statics::NewProp_AIServerURL = { "AIServerURL", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAI_ChangingSettings, AIServerURL), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AIServerURL_MetaData), NewProp_AIServerURL_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UAI_ChangingSettings_Statics::NewProp_ChatCompletionsEndpoint = { "ChatCompletionsEndpoint", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAI_ChangingSettings, ChatCompletionsEndpoint), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ChatCompletionsEndpoint_MetaData), NewProp_ChatCompletionsEndpoint_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAI_ChangingSettings_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAI_ChangingSettings_Statics::NewProp_AIServerURL,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAI_ChangingSettings_Statics::NewProp_ChatCompletionsEndpoint,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAI_ChangingSettings_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAI_ChangingSettings_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UDeveloperSettings,
	(UObject* (*)())Z_Construct_UPackage__Script_AI_Changing,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAI_ChangingSettings_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAI_ChangingSettings_Statics::ClassParams = {
	&UAI_ChangingSettings::StaticClass,
	"EditorPerProjectUserSettings",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAI_ChangingSettings_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAI_ChangingSettings_Statics::PropPointers),
	0,
	0x001000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAI_ChangingSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UAI_ChangingSettings_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAI_ChangingSettings()
{
	if (!Z_Registration_Info_UClass_UAI_ChangingSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAI_ChangingSettings.OuterSingleton, Z_Construct_UClass_UAI_ChangingSettings_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAI_ChangingSettings.OuterSingleton;
}
template<> AI_CHANGING_API UClass* StaticClass<UAI_ChangingSettings>()
{
	return UAI_ChangingSettings::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAI_ChangingSettings);
UAI_ChangingSettings::~UAI_ChangingSettings() {}
// End Class UAI_ChangingSettings

// Begin Registration
struct Z_CompiledInDeferFile_FID_ue_plu__AAAAAAAAAAAA_AI_Changing_AI_Changing_HostProject_Plugins_AI_Changing_Source_AI_Changing_Public_AI_ChangingSettings_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAI_ChangingSettings, UAI_ChangingSettings::StaticClass, TEXT("UAI_ChangingSettings"), &Z_Registration_Info_UClass_UAI_ChangingSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAI_ChangingSettings), 13994233U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_ue_plu__AAAAAAAAAAAA_AI_Changing_AI_Changing_HostProject_Plugins_AI_Changing_Source_AI_Changing_Public_AI_ChangingSettings_h_3702697482(TEXT("/Script/AI_Changing"),
	Z_CompiledInDeferFile_FID_ue_plu__AAAAAAAAAAAA_AI_Changing_AI_Changing_HostProject_Plugins_AI_Changing_Source_AI_Changing_Public_AI_ChangingSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_ue_plu__AAAAAAAAAAAA_AI_Changing_AI_Changing_HostProject_Plugins_AI_Changing_Source_AI_Changing_Public_AI_ChangingSettings_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
