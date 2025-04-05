// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_ChangingStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

TSharedPtr<FSlateStyleSet> FAI_ChangingStyle::StyleInstance = nullptr;

void FAI_ChangingStyle::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("初始化AI聊天插件样式"));
	
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
		UE_LOG(LogTemp, Warning, TEXT("AI聊天插件样式已注册"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI聊天插件样式已存在"));
	}
}

void FAI_ChangingStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FAI_ChangingStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AI_ChangingStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FAI_ChangingStyle::Get()
{
	check(StyleInstance.IsValid());
	return StyleInstance.ToSharedRef();
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FAI_ChangingStyle::Create()
{
	UE_LOG(LogTemp, Warning, TEXT("创建AI聊天插件样式"));
	
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	
	// 设置内容根目录为插件Resources目录
	FString PluginDir = IPluginManager::Get().FindPlugin("AI_Changing")->GetBaseDir();
	FPaths::NormalizeDirectoryName(PluginDir);
	FString ResourcesDir = FPaths::Combine(PluginDir, TEXT("Resources"));
	Style->SetContentRoot(ResourcesDir);
	UE_LOG(LogTemp, Warning, TEXT("设置样式内容根目录为插件Resources目录: %s"), *ResourcesDir);
	
	// 方式1：使用相对路径方式加载图标
	FString RelativeIconPath = TEXT("Icon128");
	FString RelativeIconPath16 = TEXT("Icon128");
	
	// 不同命名的图标用于不同UI位置
	Style->Set(
		FName(TEXT("AI_Changing.OpenAIDialog")), 
		new FSlateImageBrush(FPaths::Combine(ResourcesDir, RelativeIconPath + TEXT(".png")), Icon40x40)
	);
	Style->Set(
		FName(TEXT("AI_Changing.OpenAIDialog.Small")), 
		new FSlateImageBrush(FPaths::Combine(ResourcesDir, RelativeIconPath16 + TEXT(".png")), Icon16x16)
	);
	
	// 方式2：使用完整路径加载图标
	FString IconPath40 = FPaths::Combine(ResourcesDir, TEXT("AI_one.png"));
	FString IconPath16 = FPaths::Combine(ResourcesDir, TEXT("AI_one.png"));
	
	// 对小图标设置特殊命名，用于工具栏和菜单
	Style->Set(
		FName(TEXT("AI_Changing.ToolbarIcon")), 
		new FSlateImageBrush(IconPath16, Icon16x16)
	);
	Style->Set(
		FName(TEXT("AI_Changing.MenuIcon")), 
		new FSlateImageBrush(IconPath40, Icon40x40)
	);
	
	UE_LOG(LogTemp, Warning, TEXT("已设置多种图标样式，确保至少一种能正常显示彩色"));
	UE_LOG(LogTemp, Warning, TEXT("AI聊天插件样式创建完成"));
	return Style;
} 