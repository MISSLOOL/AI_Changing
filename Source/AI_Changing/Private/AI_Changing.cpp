// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_Changing.h"
#include "AI_ChangingStyle.h"
#include "AI_ChangingSettings.h"
#include "FixAIChanging.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "EditorStyleSet.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/CoreDelegates.h"
#include "HAL/PlatformTime.h"
// 添加引擎版本定义
#include "Runtime/Launch/Resources/Version.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateBasics.h"
#include "Widgets/Views/SListView.h"
#include "Framework/Text/TextLayout.h"
#include "Http.h"

// 添加Windows API支持
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include <ShlObj.h>  // 添加SHGetFolderPath所需的头文件
#include "Windows/HideWindowsPlatformTypes.h"
#endif

static const FName AI_ChangingTabName(TEXT("AIChat"));

#define LOCTEXT_NAMESPACE "FAI_ChangingModule"

void FAI_ChangingCommands::RegisterCommands()
{
	UE_LOG(LogTemp, Warning, TEXT("正在注册AI聊天命令"));
	UI_COMMAND(OpenAIDialogCommand, "AI Chat", "Open AI Chat Dialog", EUserInterfaceActionType::Button, FInputChord());
	UE_LOG(LogTemp, Warning, TEXT("AI聊天命令已注册：%s"), OpenAIDialogCommand.IsValid() ? TEXT("成功") : TEXT("失败"));
}

void FAI_ChangingModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("AI聊天插件开始启动"));
	
	// 初始化样式 - 避免关闭再初始化，可能导致崩溃
	FAI_ChangingStyle::Initialize();
	UE_LOG(LogTemp, Warning, TEXT("AI聊天样式初始化完成"));
	
	// Register the commands
	FAI_ChangingCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FAI_ChangingCommands::Get().OpenAIDialogCommand,
		FExecuteAction::CreateRaw(this, &FAI_ChangingModule::PluginButtonClicked),
		FCanExecuteAction());
	
	UE_LOG(LogTemp, Warning, TEXT("AI聊天插件命令已注册"));

	// Setup available models
	// 添加主要/常用模型
	AvailableModels.Add(MakeShareable(new FString(TEXT("deepseek-r1:1.5b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("llama3"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("llama3:8b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("qwen:14b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("qwen:4b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("mistral"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("gemma:7b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("gemma:2b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("mixtral"))));
	
	// 添加特定用途模型
	AvailableModels.Add(MakeShareable(new FString(TEXT("deepseek-coder"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("deepseek-chat"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("codellama"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("codellama:7b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("wizardcoder"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("phi3"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("phi3:3.8b"))));
	
	// 添加其他通用模型
	AvailableModels.Add(MakeShareable(new FString(TEXT("stablelm"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("neural-chat"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("openchat"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("orca-mini"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("vicuna"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("yi"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("dolphin-mistral"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("starling-lm"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("llama2"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("llama2:13b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("nous-hermes"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("zephyr"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("falcon:7b"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("phi2"))));
	AvailableModels.Add(MakeShareable(new FString(TEXT("solar"))));

	// 确保在引擎初始化后注册标签页和菜单
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		RegisterTabSpawner();
		
		// 注册菜单
		FToolMenusHelper::EnsureToolMenusInitialized();
		RegisterMenus();
		
		UE_LOG(LogTemp, Warning, TEXT("AI聊天插件完成PostEngineInit初始化"));
		
		// 添加扩展到级别编辑器菜单
		if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
		{
			FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
			
			// 创建菜单扩展
			TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
			MenuExtender->AddMenuExtension(
				"WindowLayout",
				EExtensionHook::After,
				PluginCommands,
				FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder)
				{
					// 显式使用PluginButtonClicked作为点击处理函数
					FUIAction AIAction(
						FExecuteAction::CreateRaw(this, &FAI_ChangingModule::PluginButtonClicked),
						FCanExecuteAction()
					);
					
					MenuBuilder.AddMenuEntry(
						FText::FromString(TEXT("AI聊天")),
						FText::FromString(TEXT("打开AI聊天窗口")),
						FSlateIcon(FAI_ChangingStyle::GetStyleSetName(), "AI_Changing.MenuIcon"),
						AIAction,
						NAME_None,
						EUserInterfaceActionType::Button
					);
					UE_LOG(LogTemp, Warning, TEXT("AI聊天扩展已添加到菜单，显式绑定到PluginButtonClicked"));
				})
			);
			
			if (LevelEditorModule.GetMenuExtensibilityManager().IsValid())
			{
				LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
				UE_LOG(LogTemp, Warning, TEXT("菜单扩展器已添加到级别编辑器"));
			}
		}
		
		// 使用委托代替计时器进行检查
		RegisterPeriodicCheck();
	});
	
	UE_LOG(LogTemp, Warning, TEXT("AI聊天插件启动完成"));
}

void FAI_ChangingModule::ShutdownModule()
{
	// 关闭所有Ollama进程
	CleanupOllamaProcesses();

	// Unregister the tab spawner
	UnregisterTabSpawner();

	// Shutdown the style
	FAI_ChangingStyle::Shutdown();
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (UToolMenus::IsToolMenuUIEnabled())
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}

	FAI_ChangingCommands::Unregister();
}

void FAI_ChangingModule::RegisterTabSpawner()
{
	UE_LOG(LogTemp, Warning, TEXT("正在注册AI聊天标签页生成器"));
	
	// 检查：确保FGlobalTabmanager已初始化
	TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	if (!TabManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FGlobalTabmanager未初始化，无法注册标签生成器"));
		return;
	}
	
	// 先注销现有的生成器
	if (TabManager->HasTabSpawner(AI_ChangingTabName))
	{
		UE_LOG(LogTemp, Warning, TEXT("检测到已存在的标签生成器，先注销"));
		TabManager->UnregisterNomadTabSpawner(AI_ChangingTabName);
	}
	
	// 使用NomadTab类型注册标签生成器
	TabManager->RegisterNomadTabSpawner(
		AI_ChangingTabName,
		FOnSpawnTab::CreateRaw(this, &FAI_ChangingModule::OnSpawnPluginTab)
	)
	.SetDisplayName(LOCTEXT("AI_ChangingTabTitle", "AI Chat"))
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FAI_ChangingStyle::GetStyleSetName(), "AI_Changing.MenuIcon"));
	
	UE_LOG(LogTemp, Warning, TEXT("AI聊天标签页生成器注册成功"));
}

void FAI_ChangingModule::UnregisterTabSpawner()
{
	UE_LOG(LogTemp, Warning, TEXT("正在注销AI聊天标签页生成器"));
	
	// 检查：确保FGlobalTabmanager已初始化
	TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	if (!TabManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FGlobalTabmanager未初始化，无法注销标签生成器"));
		return;
	}
	
	if (TabManager->HasTabSpawner(AI_ChangingTabName))
	{
		TabManager->UnregisterNomadTabSpawner(AI_ChangingTabName);
		UE_LOG(LogTemp, Warning, TEXT("AI聊天标签页生成器已注销"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("没有找到AI聊天标签页生成器，无需注销"));
	}
}

void FAI_ChangingModule::PluginButtonClicked()
{
	// 添加调试信息
	UE_LOG(LogTemp, Warning, TEXT("**** AI Chat按钮被点击，尝试打开AI聊天标签页 ****"));
	
	// 显示通知消息
	FNotificationInfo Info(FText::FromString(TEXT("正在打开AI聊天窗口...")));
	Info.bFireAndForget = true;
	Info.FadeInDuration = 0.5f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 2.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
	
	// 确保先注销旧的标签生成器，再重新注册
	UE_LOG(LogTemp, Warning, TEXT("开始注销和重新注册标签生成器..."));
	UnregisterTabSpawner();
	RegisterTabSpawner();
	
	// 获取全局TabManager
	TSharedPtr<FGlobalTabmanager> TabManager = FGlobalTabmanager::Get();
	if (!TabManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FGlobalTabmanager无效，无法打开标签页"));
		return;
	}
	
	// 检查标签生成器是否已注册
	if (TabManager->HasTabSpawner(AI_ChangingTabName))
	{
		UE_LOG(LogTemp, Warning, TEXT("找到标签生成器，正在调用标签页..."));
		// 使用TryInvokeTab代替InvokeTab，因为某些UE版本不支持InvokeTab
		TSharedPtr<SDockTab> AIChatTab = TabManager->TryInvokeTab(AI_ChangingTabName);
		if (AIChatTab.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("成功打开AI聊天标签页，Tab有效"));
			
			// 确保Tab可见
			AIChatTab->FlashTab();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("无法打开AI聊天标签页，TryInvokeTab返回无效指针"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("未找到标签生成器，无法打开AI聊天窗口"));
	}
}

void FAI_ChangingModule::RegisterMenus()
{
	// 添加调试信息
	UE_LOG(LogTemp, Warning, TEXT("正在注册AI聊天插件菜单"));
	
	// Register menu extensions
	if (!UToolMenus::IsToolMenuUIEnabled())
	{
		UE_LOG(LogTemp, Error, TEXT("UToolMenus未启用，无法注册菜单"));
		return;
	}

	// Register owner for proper cleanup
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([this]()
	{
		// Add toolbar extension to multiple possible toolbars to ensure visibility
		TArray<FName> ToolbarMenuNames = {
			"LevelEditor.LevelEditorToolBar",
			"LevelEditor.LevelEditorToolBar.PlayToolBar",
			"LevelEditor.MainMenu"
		};
		
		for (const FName& MenuName : ToolbarMenuNames)
		{
			UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(MenuName);
			if (ToolbarMenu)
			{
				UE_LOG(LogTemp, Warning, TEXT("在菜单 %s 中添加AI按钮"), *MenuName.ToString());
				FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("AI");
				
				// 关键修改：确保工具栏按钮使用正确的UICommand绑定方式
				Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					"AI_Chat_Command",
					FUIAction(
						FExecuteAction::CreateRaw(this, &FAI_ChangingModule::PluginButtonClicked),
						FCanExecuteAction()
					),
					FText::FromString(TEXT("AI Chat")),
					LOCTEXT("AI_ChangingToolTip", "Open AI Chat Window"),
					FSlateIcon(FAI_ChangingStyle::GetStyleSetName(), "AI_Changing.ToolbarIcon")
				));
				
				UE_LOG(LogTemp, Warning, TEXT("已在工具栏 %s 添加AI按钮，直接绑定到PluginButtonClicked"), *MenuName.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("无法扩展菜单: %s"), *MenuName.ToString());
			}
		}
		
		// Also add to a custom section on the main menu for additional visibility
		UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
		if (MainMenu)
		{
			FToolMenuSection& Section = MainMenu->FindOrAddSection("AI Tools");
			Section.AddMenuEntry(
				"OpenAIChat",
				FText::FromString(TEXT("打开AI聊天")),
				FText::FromString(TEXT("与本地AI模型进行对话")),
				FSlateIcon(FAI_ChangingStyle::GetStyleSetName(), "AI_Changing.MenuIcon"),
				FUIAction(FExecuteAction::CreateRaw(this, &FAI_ChangingModule::PluginButtonClicked), FCanExecuteAction())
			);
			UE_LOG(LogTemp, Warning, TEXT("已添加AI聊天到主菜单，并直接绑定到PluginButtonClicked"));
		}
	}));
	
	UE_LOG(LogTemp, Warning, TEXT("AI聊天插件菜单注册完成"));
}

TSharedRef<SDockTab> FAI_ChangingModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// 添加调试信息
	UE_LOG(LogTemp, Warning, TEXT("正在生成AI聊天标签页"));
	
	// 显示通知消息
	FNotificationInfo Info(FText::FromString(TEXT("AI聊天窗口已打开")));
	Info.bFireAndForget = true;
	Info.FadeInDuration = 0.5f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 2.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
	
	// 创建输入框
	TSharedRef<SEditableTextBox> PromptInputBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("PromptInputHint", "Enter your prompt here..."))
		.MinDesiredWidth(300.0f);
	
	// 创建模型选择下拉菜单
	TSharedRef<SComboBox<TSharedPtr<FString>>> ModelSelectorLocal =
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&AvailableModels)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem) {
			return SNew(STextBlock)
				.Text(FText::FromString(*InItem))
				.ToolTipText(FText::FromString(*InItem));
		})
		.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InItem, ESelectInfo::Type SelectType) {
			// 处理模型选择
			if (InItem.IsValid())
			{
				ModelSelector->SetSelectedItem(InItem);
			}
		})
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("选择模型")))
			.MinDesiredWidth(120.0f)
		];

	ModelSelector = ModelSelectorLocal;

	// Select the first model by default
	if (AvailableModels.Num() > 0)
	{
		ModelSelector->SetSelectedItem(AvailableModels[0]);
	}
	
	// 创建可以动态更新的模型选择器容器
	ModelSelectorContainer = SNew(SBox);
	ModelSelectorContainer->SetContent(ModelSelector.ToSharedRef());

	// 初始化聊天消息数组和列表视图
	ChatMessages.Empty();
	
	// 添加搜索模型功能
	TSharedRef<SEditableTextBox> ModelSearchBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("ModelSearchHint", "搜索模型..."))
		.OnTextChanged_Lambda([this](const FText& NewText) {
			// 实现模型搜索逻辑
			FString SearchString = NewText.ToString().ToLower();
			
			// 如果搜索框为空，显示所有模型
			if (SearchString.IsEmpty())
			{
				// 创建使用完整模型列表的下拉框
				TSharedRef<SComboBox<TSharedPtr<FString>>> NewModelSelector = 
					SNew(SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&AvailableModels)
					.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem) {
						return SNew(STextBlock)
							.Text(FText::FromString(*InItem))
							.ToolTipText(FText::FromString(*InItem));
					})
					.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InItem, ESelectInfo::Type SelectType) {
						// 处理模型选择
						if (InItem.IsValid())
						{
							ModelSelector->SetSelectedItem(InItem);
						}
					})
					.Content()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("选择模型")))
						.ToolTipText(LOCTEXT("ModelSelectorTooltip", "选择要使用的AI模型"))
						.MinDesiredWidth(120.0f)
					];
				
				// 更新ModelSelector引用
				ModelSelector = NewModelSelector;
				
				// 如果有预选模型，保持选中
				if (AvailableModels.Num() > 0)
				{
					ModelSelector->SetSelectedItem(AvailableModels[0]);
				}
				
				// 更新容器内容
				if (ModelSelectorContainer.IsValid())
				{
					ModelSelectorContainer->SetContent(ModelSelector.ToSharedRef());
				}
				
				return;
			}
			
			// 否则过滤模型
			FilteredModels.Empty();
			
			for (const TSharedPtr<FString>& Model : AvailableModels)
			{
				if (Model->ToLower().Contains(SearchString))
				{
					FilteredModels.Add(Model);
				}
			}
			
			// 如果过滤后没有模型，不更新
			if (FilteredModels.Num() == 0)
			{
				return;
			}
			
			// 创建新的下拉列表以使用过滤后的数据源
			TSharedRef<SComboBox<TSharedPtr<FString>>> FilteredSelector = 
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&FilteredModels)
				.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem) {
					return SNew(STextBlock)
						.Text(FText::FromString(*InItem))
						.ToolTipText(FText::FromString(*InItem));
				})
				.OnSelectionChanged_Lambda([this](TSharedPtr<FString> InItem, ESelectInfo::Type SelectType) {
					// 处理模型选择
					if (InItem.IsValid())
					{
						ModelSelector->SetSelectedItem(InItem);
					}
				})
				.Content()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("选择模型")))
					.ToolTipText(LOCTEXT("ModelSelectorTooltip", "选择要使用的AI模型"))
					.MinDesiredWidth(120.0f)
				];
			
			// 更新ModelSelector引用
			ModelSelector = FilteredSelector;
			
			// 如果有过滤结果，选择第一个
			if (FilteredModels.Num() > 0)
			{
				ModelSelector->SetSelectedItem(FilteredModels[0]);
			}
			
			// 更新容器内容
			if (ModelSelectorContainer.IsValid())
			{
				ModelSelectorContainer->SetContent(ModelSelector.ToSharedRef());
			}
		});
	
	ChatListView = SNew(SListView<TSharedPtr<FChatMessage>>)
		// ItemHeight在新版UE中仅用于Tile视图，改用SetItemHeight方式
		.ListItemsSource(&ChatMessages)
		.OnGenerateRow_Lambda([this](TSharedPtr<FChatMessage> Message, const TSharedRef<STableViewBase>& OwnerTable) {
			// 创建可选择文本
			TSharedRef<SEditableTextBox> EditableText = SNew(SEditableTextBox)
				.Text(FText::FromString(Message->Content))
				.IsReadOnly(true)
				.MinDesiredWidth(400.0f)
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11));
			
			if (Message->bIsError)
			{
				EditableText->SetForegroundColor(FLinearColor::Red);
			}
			
			// 添加复制按钮
			TSharedRef<SButton> CopyButton = SNew(SButton)
				.ToolTipText(FText::FromString(TEXT("复制到剪贴板")))
				.ButtonStyle(FCoreStyle::Get(), "NoBorder")
				.ContentPadding(FMargin(4.0f, 2.0f))
				.OnClicked_Lambda([Content = Message->Content]()
				{
					// 使用Windows API进行剪贴板复制
					#if PLATFORM_WINDOWS
					if (OpenClipboard(NULL))
					{
						EmptyClipboard();
						
						// 获取UTF-16字符串
						FString WideMessage = Content;
						int32 WideLen = WideMessage.Len() + 1; // 包含最后的\0
						
						// 分配全局内存
						HGLOBAL GlobalMem = GlobalAlloc(GMEM_MOVEABLE, WideLen * sizeof(TCHAR));
						if (GlobalMem)
						{
							// 复制数据
							TCHAR* Buffer = (TCHAR*)GlobalLock(GlobalMem);
							if (Buffer)
							{
								FCString::Strcpy(Buffer, WideLen, *WideMessage);
								GlobalUnlock(GlobalMem);
								SetClipboardData(CF_UNICODETEXT, GlobalMem);
							}
						}
						
						CloseClipboard();
					}
					#endif
					
					// 显示复制成功通知
					FNotificationInfo Info(FText::FromString(TEXT("已复制到剪贴板")));
					Info.bFireAndForget = true;
					Info.FadeInDuration = 0.2f;
					Info.FadeOutDuration = 0.5f;
					Info.ExpireDuration = 1.5f;
					FSlateNotificationManager::Get().AddNotification(Info);
					
					return FReply::Handled();
				})
				.Content()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("📋")))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				];
			
			// 创建整个消息行
			return SNew(STableRow<TSharedPtr<FChatMessage>>, OwnerTable)
				.Style(FCoreStyle::Get(), "TableView.Row")
				.Padding(FMargin(5, 10, 5, 10)) // 为每一行添加足够的垂直间距
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(0, 0, 10, 0) // 增加发送者与消息内容之间的间距
					[
						SNew(STextBlock)
						.Text(FText::FromString(Message->Sender + TEXT(": ")))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
						.ColorAndOpacity(Message->bIsError ? FLinearColor::Red : FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							EditableText
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Top)
						.Padding(5, 0, 0, 0)
						[
							CopyButton
						]
					]
				];
		})
		.SelectionMode(ESelectionMode::None); // 禁用选择

	// 保留旧的聊天历史容器的引用，以便向后兼容
	ChatHistoryBox = SNew(SVerticalBox);
	
	// 创建主布局
	TSharedRef<SVerticalBox> MainLayout = SNew(SVerticalBox);
	
	// 添加聊天历史区域
	MainLayout->AddSlot()
	.FillHeight(1.0f)
	.Padding(8)
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.Padding(4)
		[
			ChatListView.ToSharedRef()
		]
	];
	
	// 添加输入区域
	MainLayout->AddSlot()
	.AutoHeight()
	.Padding(8, 0, 8, 8)
	[
		SNew(SVerticalBox)
		
		// 模型选择行
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 5)
		[
			SNew(SHorizontalBox)
			
			// 模型搜索框
			+ SHorizontalBox::Slot()
			.FillWidth(0.25f)
			.Padding(0, 0, 5, 0)
			.VAlign(VAlign_Center)
			[
				ModelSearchBox
			]
			
			// 模型选择下拉框
			+ SHorizontalBox::Slot()
			.FillWidth(0.25f)
			.Padding(0, 0, 5, 0)
			.VAlign(VAlign_Center)
			[
				ModelSelectorContainer.ToSharedRef()
			]
			
			// 启动服务按钮
			+ SHorizontalBox::Slot()
			.FillWidth(0.1f)
			.Padding(0, 0, 5, 0)
			[
				SNew(SButton)
				.Text(LOCTEXT("StartServerButton", "启动服务"))
				.HAlign(HAlign_Center)
				.ToolTipText(LOCTEXT("StartServerTooltip", "启动Ollama服务并加载所选模型"))
				.OnClicked(FOnClicked::CreateLambda([this]() -> FReply {
					if (ModelSelector.IsValid() && ModelSelector->GetSelectedItem().IsValid())
					{
						FString SelectedModel = *ModelSelector->GetSelectedItem().Get();
						StartOllamaService(SelectedModel);
					}
					else
					{
						AddMessageToChat(TEXT("System"), TEXT("请先选择一个模型"), true);
					}
					return FReply::Handled();
				}))
			]
			
			// 停止服务按钮
			+ SHorizontalBox::Slot()
			.FillWidth(0.1f)
			.Padding(0, 0, 5, 0)
			[
				SNew(SButton)
				.Text(LOCTEXT("StopServerButton", "停止服务"))
				.HAlign(HAlign_Center)
				.ToolTipText(LOCTEXT("StopServerTooltip", "停止Ollama服务并终止所有下载"))
				.OnClicked(FOnClicked::CreateLambda([this]() -> FReply {
					// 清理所有Ollama进程
					CleanupOllamaProcesses();
					
					// 添加停止消息
					AddMessageToChat(TEXT("System"), TEXT("已停止所有Ollama服务和下载"));
					
					// 移除所有可能进行中的进度消息
					TArray<TSharedPtr<FChatMessage>> MessagesToRemove;
					for (TSharedPtr<FChatMessage> Message : ChatMessages)
					{
						if (Message->Sender == TEXT("System") && 
							(Message->Content.Contains(TEXT("正在下载模型")) || 
							 Message->Content.Contains(TEXT("正在准备模型")) ||
							 Message->Content.Contains(TEXT("正在加载模型"))))
						{
							MessagesToRemove.Add(Message);
						}
					}
					
					// 移除进度消息
					for (TSharedPtr<FChatMessage> Message : MessagesToRemove)
					{
						ChatMessages.Remove(Message);
					}
					
					// 刷新UI
					if (ChatListView.IsValid() && MessagesToRemove.Num() > 0)
					{
						ChatListView->RequestListRefresh();
					}
					
					return FReply::Handled();
				}))
			]
		]
		
		// 输入和发送行
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			
			// 输入框
			+ SHorizontalBox::Slot()
			.FillWidth(0.9f)
			.Padding(0, 0, 5, 0)
			[
				PromptInputBox
			]
			
			// 发送按钮
			+ SHorizontalBox::Slot()
			.FillWidth(0.1f)
			[
				SNew(SButton)
				.Text(LOCTEXT("SendButton", "发送"))
				.HAlign(HAlign_Center)
				.OnClicked(FOnClicked::CreateLambda([PromptInputBoxPtr = PromptInputBox, this]() -> FReply {
					FString Prompt = PromptInputBoxPtr->GetText().ToString();
					if (!Prompt.IsEmpty() && ModelSelector.IsValid() && ModelSelector->GetSelectedItem().IsValid())
					{
						// 添加用户消息到聊天
						AddMessageToChat(TEXT("User"), Prompt);
						
						// Send the request to local AI
						SendAIRequest(Prompt, *ModelSelector->GetSelectedItem().Get());
						
						// Clear the input box
						PromptInputBoxPtr->SetText(FText::GetEmpty());
					}
					return FReply::Handled();
				}))
			]
		]
	];

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			MainLayout
		];
}

TArray<TSharedPtr<FString>> FAI_ChangingModule::GetAvailableModels() const
{
	return AvailableModels;
}

void FAI_ChangingModule::SendAIRequest(const FString& Prompt, const FString& SelectedModel)
{
	// 检查模型名称是否有效，如果无效则使用默认值"AI"
	FString SenderName = SelectedModel;
	if (SenderName.IsEmpty() || SenderName.Equals(TEXT("选择模型")))
	{
		SenderName = TEXT("AI");
	}

	// 添加"正在思考"消息
	TSharedPtr<FChatMessage> TypingMessage = MakeShareable(new FChatMessage(SenderName, TEXT("<think>"), false));
	ChatMessages.Add(TypingMessage);
	
	// 刷新列表并滚动到底部
	if (ChatListView.IsValid())
	{
		ChatListView->RequestListRefresh();
		ChatListView->RequestScrollIntoView(TypingMessage);
	}
	
	// Creating HTTP request
	HTTPRequest = FHttpModule::Get().CreateRequest();
	
	// Get URL from settings
	const UAI_ChangingSettings* Settings = GetDefault<UAI_ChangingSettings>();
	FString RequestURL = Settings->GetChatCompletionsURL();
	
	HTTPRequest->SetURL(RequestURL);
	HTTPRequest->SetVerb(TEXT("POST"));
	HTTPRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	// Ollama不需要API密钥
	
	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("model"), SelectedModel);
	
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	
	// Create message object
	TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
	MessageObject->SetStringField(TEXT("role"), TEXT("user"));
	MessageObject->SetStringField(TEXT("content"), Prompt);
	MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
	
	JsonObject->SetArrayField(TEXT("messages"), MessagesArray);
	// Ollama特定设置
	JsonObject->SetBoolField(TEXT("stream"), false);
	
	// Convert JSON object to string
	FString RequestString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	// Set request content
	HTTPRequest->SetContentAsString(RequestString);
	
	// Set response callback
	HTTPRequest->OnProcessRequestComplete().BindLambda([this, SelectedModelCopy = SelectedModel, SenderNameCopy = SenderName, TypingMessageCopy = TypingMessage](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess) {
		// 移除"思考中"消息
		if (ChatMessages.Contains(TypingMessageCopy))
		{
			ChatMessages.Remove(TypingMessageCopy);
		}
		
		if (bSuccess && Response.IsValid())
		{
			FString ResponseStr = Response->GetContentAsString();
			
			// Parse JSON response
			TSharedPtr<FJsonObject> JsonResponse;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
			
			if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
			{
				FString Content;
				
				// 适配Ollama的响应格式
				if (JsonResponse->HasField(TEXT("message")) && JsonResponse->GetObjectField(TEXT("message"))->HasField(TEXT("content")))
				{
					Content = JsonResponse->GetObjectField(TEXT("message"))->GetStringField(TEXT("content"));
				}
				// 尝试标准OpenAI格式作为备选
				else if (JsonResponse->HasField(TEXT("choices")) && JsonResponse->GetArrayField(TEXT("choices")).Num() > 0)
				{
					TSharedPtr<FJsonObject> Choice = JsonResponse->GetArrayField(TEXT("choices"))[0]->AsObject();
					if (Choice->HasField(TEXT("message")) && Choice->GetObjectField(TEXT("message"))->HasField(TEXT("content")))
					{
						Content = Choice->GetObjectField(TEXT("message"))->GetStringField(TEXT("content"));
					}
				}
				
				// 如果仍未能解析内容，直接使用原始响应
				if (Content.IsEmpty())
				{
					UE_LOG(LogTemp, Warning, TEXT("尝试直接解析原始响应: %s"), *ResponseStr);
					
					// 尝试清理特殊标记
					ResponseStr.ReplaceInline(TEXT("<think>"), TEXT(""));
					ResponseStr.ReplaceInline(TEXT("</think>"), TEXT(""));
					ResponseStr.ReplaceInline(TEXT("<\\think>"), TEXT(""));
					ResponseStr.ReplaceInline(TEXT("<∕think>"), TEXT(""));
					ResponseStr.ReplaceInline(TEXT("\n\n"), TEXT("\n"));
					
					Content = ResponseStr;
					if (Content.IsEmpty())
					{
						Content = TEXT("(收到空响应)");
					}
				}
				
				if (!Content.IsEmpty())
				{
					// 使用辅助方法添加模型消息，使用模型名称代替"AI"
					AddMessageToChat(SenderNameCopy, Content);
				}
			}
			else
			{
				// 解析失败，显示原始响应并添加错误提示
				FString ErrorContent = TEXT("无法解析服务器响应。原始响应:\n\n") + ResponseStr;
				if (ErrorContent.IsEmpty())
				{
					ErrorContent = TEXT("服务器返回了空响应");
				}
				
				// 创建可选择的文本和复制按钮
				AddMessageToChat(TEXT("System"), ErrorContent, true);
			}
		}
		else
		{
			// 连接错误
			FString ErrorMessage = TEXT("无法连接到AI服务器。请确保Ollama服务正在运行，并检查以下内容：\n");
			ErrorMessage += TEXT("1. 确认已运行 ollama run ") + SelectedModelCopy + TEXT("\n");
			ErrorMessage += TEXT("2. 确认服务运行在端口11434上\n");
			ErrorMessage += TEXT("3. 检查防火墙设置是否允许连接");
			
			AddMessageToChat(TEXT("System"), ErrorMessage, true);
		}
		
		// 在所有情况下都要刷新列表视图
		if (ChatListView.IsValid())
		{
			ChatListView->RequestListRefresh();
			if (ChatMessages.Num() > 0)
			{
				ChatListView->RequestScrollIntoView(ChatMessages.Last());
			}
		}
	});
	
	// Send request
	HTTPRequest->ProcessRequest();
}

// 辅助方法：添加消息到聊天
void FAI_ChangingModule::AddMessageToChat(const FString& Sender, const FString& Message, bool bIsError)
{
	TSharedPtr<FChatMessage> NewMessage = MakeShareable(new FChatMessage(Sender, Message, bIsError));
	ChatMessages.Add(NewMessage);
	
	// 刷新列表并滚动到底部
	if (ChatListView.IsValid())
	{
		ChatListView->RequestListRefresh();
		ChatListView->RequestScrollIntoView(NewMessage);
	}
}

// 周期性检查状态全局变量
namespace AIChangingTimerState
{
	static int32 CheckCount = 0;
	static double LastCheckTime = 0.0;
}

// 添加进度监控状态控制
namespace OllamaProgressMonitor
{
	static bool bShouldStopMonitoring = false;
}

// 启动Ollama服务和运行模型的方法
void FAI_ChangingModule::StartOllamaService(const FString& ModelName)
{
	// 防止重复点击启动
	if (bIsServiceStarting)
	{
		AddMessageToChat(TEXT("System"), TEXT("Ollama服务正在启动中，请稍候..."));
		return;
	}
	
	// 设置正在启动标志
	bIsServiceStarting = true;
	
	// 构建Ollama可执行文件的完整路径
	FString OllamaPath = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/AI_Changing/Resources/AI_ZD/ollama/ollama.exe"))
	);
	
	// 检查文件是否存在
	if (!FPaths::FileExists(OllamaPath))
	{
		AddMessageToChat(TEXT("System"), FString::Printf(TEXT("找不到Ollama可执行文件，路径: %s"), *OllamaPath), true);
		bIsServiceStarting = false; // 释放标志，允许再次尝试
		return;
	}
	
	// 添加启动状态消息
	AddMessageToChat(TEXT("System"), FString::Printf(TEXT("正在启动Ollama服务和加载模型: %s..."), *ModelName));
	
	// 创建进程通信管道
	void* PipeRead = nullptr;
	void* PipeWrite = nullptr;
	
	// 确保进程通信管道创建成功
	if (!FPlatformProcess::CreatePipe(PipeRead, PipeWrite))
	{
		AddMessageToChat(TEXT("System"), TEXT("无法创建进程通信管道"), true);
		bIsServiceStarting = false; // 释放标志，允许再次尝试
		return;
	}
	
	// 启动Ollama服务进程
	FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
		*OllamaPath,
		TEXT("serve"),
		false,  // 不在独立窗口显示
		true,   // 隐藏窗口
		true,   // 在后台运行
		nullptr,
		0,
		nullptr,
		PipeWrite
	);
	
	// 检查服务是否成功启动
	if (!ProcessHandle.IsValid())
	{
		AddMessageToChat(TEXT("System"), TEXT("无法启动Ollama服务，请检查以下内容:"), true);
		AddMessageToChat(TEXT("System"), TEXT("1. 确保安装路径正确"), true);
		AddMessageToChat(TEXT("System"), TEXT("2. 检查是否有足够的权限"), true);
		AddMessageToChat(TEXT("System"), TEXT("3. 防火墙是否允许Ollama连接网络"), true);
		
		// 关闭管道并退出
		FPlatformProcess::ClosePipe(PipeRead, PipeWrite);
		bIsServiceStarting = false; // 释放标志，允许再次尝试
		return;
	}
	
	// 保存服务进程信息 - 无需获取PID
	UE_LOG(LogTemp, Warning, TEXT("Ollama服务进程已启动"));
	
	// 服务启动成功
	AddMessageToChat(TEXT("System"), TEXT("Ollama服务已成功启动"));
	
	// 等待一段时间让服务完全启动
	AddMessageToChat(TEXT("System"), TEXT("正在等待服务初始化..."));
	FPlatformProcess::Sleep(3.0f);
	
	// 创建运行模型的命令 - 使用 -v 参数启用详细输出
	FString RunModelCommand = FString::Printf(TEXT("run %s"), *ModelName);
	
	// 关闭之前创建的管道，我们不使用管道读取输出
	FPlatformProcess::ClosePipe(PipeRead, PipeWrite);
	
	// 创建一个单独的进度消息，当有进度更新时替换此消息
	TSharedPtr<FChatMessage> ProgressMessage = MakeShareable(new FChatMessage(TEXT("System"), TEXT("正在准备模型..."), false));
	ChatMessages.Add(ProgressMessage);
	if (ChatListView.IsValid())
	{
		ChatListView->RequestListRefresh();
		ChatListView->RequestScrollIntoView(ProgressMessage);
	}
	
	// 启动模型 - 重新配置为捕获输出
	void* ModelPipeRead = nullptr;
	void* ModelPipeWrite = nullptr;
	
	// 创建用于捕获模型进程输出的管道
	if (!FPlatformProcess::CreatePipe(ModelPipeRead, ModelPipeWrite))
	{
		AddMessageToChat(TEXT("System"), TEXT("无法创建模型进程通信管道"), true);
		bIsServiceStarting = false;
		return;
	}
	
	// 启动模型进程，并捕获其输出
	FProcHandle ModelProcessHandle = FPlatformProcess::CreateProc(
		*OllamaPath,
		*RunModelCommand,
		false,  // 不在独立窗口显示
		true,   // 隐藏窗口
		true,   // 在后台运行
		nullptr,
		0,
		nullptr,
		ModelPipeWrite  // 捕获标准输出
	);
	
	// 检查模型是否成功启动
	if (!ModelProcessHandle.IsValid())
	{
		AddMessageToChat(TEXT("System"), FString::Printf(TEXT("无法启动模型 %s，请检查以下内容:"), *ModelName), true);
		AddMessageToChat(TEXT("System"), TEXT("1. 确保模型已下载 (可以运行 'ollama pull " + ModelName + "' 下载)"), true);
		AddMessageToChat(TEXT("System"), TEXT("2. 检查磁盘空间是否足够"), true);
		AddMessageToChat(TEXT("System"), TEXT("3. 模型名称是否正确"), true);
		
		// 移除进度消息
		if (ChatMessages.Contains(ProgressMessage))
		{
			ChatMessages.Remove(ProgressMessage);
			if (ChatListView.IsValid())
			{
				ChatListView->RequestListRefresh();
			}
		}
		
		// 重置服务启动标志，允许再次尝试
		bIsServiceStarting = false;
		return;
	}
	else
	{
		// 添加读取模型进程输出的任务
		FCoreDelegates::OnEndFrame.AddLambda([this, ModelProcessHandle, ProgressMessage, ModelName, ModelPipeRead]()
		{
			// 检查是否应该停止监控
			if (OllamaProgressMonitor::bShouldStopMonitoring)
			{
				// 如果需要停止监控，直接移除委托并退出
				FCoreDelegates::OnEndFrame.RemoveAll(this);
				
				// 关闭管道
				if (ModelPipeRead != nullptr)
				{
					FPlatformProcess::ClosePipe(ModelPipeRead, nullptr);
				}
				
				return;
			}
			
			// 重置静态变量，保证每次启动服务时从0开始计算
			static bool bFirstRun = true;
			static int32 ConsecutiveEmptyReads = 0;
			static int32 ProgressCounter = 0;
			static bool bHasDisplayedSuccess = false; // 添加标记，确保成功消息只显示一次
			static bool bIsDownloading = false; // 添加标记，表示是否正在下载新模型
			static FString LastProgressOutput; // 存储上一次的进度输出
			
			// 用于限制消息显示频率的变量
			static float LastMessageTime = 0.0f;
			static FString LastMessageContent;
			
			// 首次运行时重置所有静态变量
			if (bFirstRun)
			{
				ConsecutiveEmptyReads = 0;
				ProgressCounter = 0;
				bHasDisplayedSuccess = false;
				bIsDownloading = false;
				LastProgressOutput = "";
				LastMessageTime = 0.0f;
				LastMessageContent = "";
				bFirstRun = false;
				
				// 检查是否需要下载模型 - 检查正确的用户模型目录
				FString ModelBaseName = ModelName;
				// 移除可能的版本号或标签 (如"llama3:8b"中的":8b")
				if (ModelBaseName.Contains(TEXT(":")))
				{
					ModelBaseName = ModelBaseName.Left(ModelBaseName.Find(TEXT(":")));
				}
				
				// 获取Windows用户目录
				FString UserDir;
				#if PLATFORM_WINDOWS
				TCHAR UserPath[MAX_PATH];
				if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, UserPath)))
				{
					UserDir = UserPath;
				}
				else
				{
					// 回退：使用环境变量
					UserDir = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("USERPROFILE")));
				}
				#else
				// 在非Windows平台上使用HOME环境变量
				UserDir = FString(FPlatformMisc::GetEnvironmentVariable(TEXT("HOME")));
				#endif
				
				// Ollama模型清单目录（最准确的检查位置）
				FString OllamaManifestsDir = FPaths::Combine(UserDir, TEXT(".ollama"), TEXT("models"), TEXT("manifests"), TEXT("registry.ollama.ai"), TEXT("library"));
				UE_LOG(LogTemp, Warning, TEXT("检查Ollama模型清单目录: %s"), *OllamaManifestsDir);
				
				// 备选检查：模型目录
				FString OllamaModelsDir = FPaths::Combine(UserDir, TEXT(".ollama"), TEXT("models"));
				
				// 多种可能的模型目录位置
				TArray<FString> PossibleModelPaths;
				
				// 1. 首先检查清单目录（最准确）
				PossibleModelPaths.Add(FPaths::Combine(OllamaManifestsDir, ModelName));
				PossibleModelPaths.Add(FPaths::Combine(OllamaManifestsDir, ModelBaseName));
				
				// 2. 备选检查常规模型目录
				PossibleModelPaths.Add(FPaths::Combine(OllamaModelsDir, ModelName));
				PossibleModelPaths.Add(FPaths::Combine(OllamaModelsDir, ModelBaseName));
				
				// 默认假设需要下载
				bIsDownloading = true;
				
				// 检查任一可能的目录是否存在
				for (const FString& Path : PossibleModelPaths)
				{
					if (FPaths::DirectoryExists(Path) || FPaths::FileExists(Path))
					{
						// 模型目录或文件已存在，不需要下载
						bIsDownloading = false;
						UE_LOG(LogTemp, Warning, TEXT("找到已存在的模型: %s"), *Path);
						break;
					}
				}
				
				if (bIsDownloading)
				{
					if (ChatMessages.Contains(ProgressMessage))
					{
						UE_LOG(LogTemp, Warning, TEXT("未找到模型 %s 的本地文件，需要下载"), *ModelName);
						ProgressMessage->Content = FString::Printf(TEXT("正在准备下载模型 %s..."), *ModelName);
						if (ChatListView.IsValid())
						{
							ChatListView->RequestListRefresh();
						}
					}
				}
				else
				{
					if (ChatMessages.Contains(ProgressMessage))
					{
						UE_LOG(LogTemp, Warning, TEXT("发现模型 %s 的本地文件，无需下载"), *ModelName);
						
						// 检查API是否已准备就绪
						FString TestPrompt = "测试连接";
						FString OllamaURL = "http://localhost:11434/api/generate";
						TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
						Request->SetURL(OllamaURL);
						Request->SetVerb("POST");
						Request->SetHeader("Content-Type", "application/json");
						
						// 构建请求体
						FString RequestBody = FString::Printf(
							TEXT("{\"model\": \"%s\", \"prompt\": \"%s\", \"stream\": false}"),
							*ModelName, *TestPrompt
						);
						Request->SetContentAsString(RequestBody);
						
						// 执行请求并等待响应
						bool bRequestSent = Request->ProcessRequest();
						
						// 等待API响应
						FPlatformProcess::Sleep(1.0f);
						
						if (bRequestSent && (Request->GetStatus() == EHttpRequestStatus::Succeeded || 
											 Request->GetStatus() == EHttpRequestStatus::Processing))
						{
							// API已准备就绪，模型已加载，无需等待
							UE_LOG(LogTemp, Warning, TEXT("模型 %s 已加载并准备就绪"), *ModelName);
							
							// 移除进度消息
							if (ChatMessages.Contains(ProgressMessage))
							{
								ChatMessages.Remove(ProgressMessage);
							}
							
							// 直接显示成功消息
							AddMessageToChat(TEXT("System"), FString::Printf(TEXT("模型 %s 已成功加载，现在可以开始聊天了！"), *ModelName));
							AddMessageToChat(TEXT("System"), TEXT("提示: 您现在可以在聊天框输入问题并点击发送按钮"));
							bHasDisplayedSuccess = true;
							
							// 重置服务启动标志，允许再次点击启动按钮
							static_cast<FAI_ChangingModule*>(FModuleManager::Get().GetModule("AI_Changing"))->ResetServiceStartingFlag();
							
							// 清理并结束委托
							if (ModelPipeRead != nullptr)
							{
								FPlatformProcess::ClosePipe(ModelPipeRead, nullptr);
							}
							FCoreDelegates::OnEndFrame.RemoveAll(this);
							return;
						}
						else
						{
							// API尚未准备就绪，继续等待
							ProgressMessage->Content = FString::Printf(TEXT("模型 %s 已存在，正在加载..."), *ModelName);
							if (ChatListView.IsValid())
							{
								ChatListView->RequestListRefresh();
							}
						}
					}
				}
			}
			
			// 进度显示逻辑
			ProgressCounter++;
			
			// 读取进程输出管道
			if (ModelPipeRead != nullptr)
			{
				// 读取频率比帧数高，确保及时捕获Ollama输出
				static float LastReadTime = 0.0f;
				float CurrentTime = FPlatformTime::Seconds();
				
				// 每0.5秒读取一次进程输出改为每0.5秒读取一次 (每秒2次)
				if ((CurrentTime - LastReadTime) > 0.5f)
				{
					LastReadTime = CurrentTime;
					
					FString Output = FPlatformProcess::ReadPipe(ModelPipeRead);
					if (!Output.IsEmpty())
					{
						LastProgressOutput = Output;
						ConsecutiveEmptyReads = 0;
						
						// 始终打印所有输出内容，帮助调试
						UE_LOG(LogTemp, Warning, TEXT("Ollama原始输出: %s"), *Output);
						
						// 过滤掉含有乱码的输出
						if (Output.Contains(TEXT("[")) && Output.Contains(TEXT("]")) && 
							Output.Contains(TEXT("?")))
						{
							// 只记录日志，不显示在UI中
							UE_LOG(LogTemp, Warning, TEXT("跳过可能包含乱码的输出"));
						}
						else
						{
							// 始终更新主进度消息
							if (ChatMessages.Contains(ProgressMessage))
							{
								// 提取可读的部分
								FString CleanOutput = Output;
								// 移除可能的乱码字符
								CleanOutput.ReplaceInline(TEXT("[?"), TEXT(""));
								CleanOutput.ReplaceInline(TEXT("["), TEXT(""));
								CleanOutput.ReplaceInline(TEXT("]"), TEXT(""));
								CleanOutput = CleanOutput.TrimEnd();
								
								// 默认状态消息
								FString StatusMessage = FString::Printf(TEXT("正在处理模型 %s..."), *ModelName);
								
								// 检查是否包含下载进度信息
								if (Output.Contains(TEXT("%")) || Output.Contains(TEXT("MiB")))
								{
									bIsDownloading = true;
									
									// 尝试提取进度百分比
									FRegexPattern Pattern(TEXT("([0-9]+\\.?[0-9]*)%"));
									FRegexMatcher Matcher(Pattern, Output);
									
									if (Matcher.FindNext())
									{
										FString PercentStr = Matcher.GetCaptureGroup(1);
										float Percent = FCString::Atof(*PercentStr);
										
										// 尝试提取下载速度
										FString SpeedInfo = TEXT("");
										// 改进速度匹配模式，更灵活地匹配各种格式
										FRegexPattern SpeedPattern(TEXT("([0-9]+\\.?[0-9]*\\s*[GMK]i?B/s)"));
										FRegexMatcher SpeedMatcher(SpeedPattern, Output);
										if (SpeedMatcher.FindNext())
										{
											SpeedInfo = SpeedMatcher.GetCaptureGroup(1);
										}
										
										// 从Ollama输出中提取文件大小信息
										FString SizeInfo = TEXT("");
										// 改进大小匹配模式，更灵活地匹配各种格式
										FRegexPattern SizePattern(TEXT("([0-9]+\\.?[0-9]*\\s*[GMK]i?B)\\s*/\\s*([0-9]+\\.?[0-9]*\\s*[GMK]i?B)"));
										FRegexMatcher SizeMatcher(SizePattern, Output);
										if (SizeMatcher.FindNext())
										{
											SizeInfo = FString::Printf(TEXT("%s/%s"), 
												*SizeMatcher.GetCaptureGroup(1), *SizeMatcher.GetCaptureGroup(2));
										}
										
										// 创建下载进度信息，使用清晰的格式显示关键信息
										if (!SpeedInfo.IsEmpty() || !SizeInfo.IsEmpty())
										{
											// 如果有速度或大小信息，使用完整格式
											FString FormattedInfo = FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent));
											
											if (!SpeedInfo.IsEmpty())
											{
												FormattedInfo += FString::Printf(TEXT(" 速度:%s"), *SpeedInfo);
											}
											
											if (!SizeInfo.IsEmpty())
											{
												FormattedInfo += FString::Printf(TEXT(" 进度:%s"), *SizeInfo);
											}
											
											StatusMessage = FormattedInfo;
										}
										else
										{
											// 如果只有百分比，就简单显示
											StatusMessage = FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent));
										}
									}
									else
									{
										StatusMessage = FString::Printf(TEXT("开始下载..."));
									}
								}
								else if (Output.Contains(TEXT("Downloading")) || Output.Contains(TEXT("下载")) || 
										Output.Contains(TEXT("downloading")) || Output.Contains(TEXT("正在下载")))
								{
									bIsDownloading = true;
									StatusMessage = FString::Printf(TEXT("开始下载..."));
								}
								else if (Output.Contains(TEXT("Loading")) || Output.Contains(TEXT("加载")) || 
										Output.Contains(TEXT("loading")))
								{
									StatusMessage = FString::Printf(TEXT("加载中..."));
								}
								else if (Output.Contains(TEXT("pulling")) || Output.Contains(TEXT("Pulling")) || 
										Output.Contains(TEXT("拉取")))
								{
									bIsDownloading = true;
									StatusMessage = FString::Printf(TEXT("开始下载..."));
								}
								
								// 更新主进度消息 - 减少UI刷新频率
								static float LastUIUpdateTime = 0.0f;
								if ((CurrentTime - LastUIUpdateTime) > 0.5f) // 每0.5秒更新一次UI
								{
									ProgressMessage->Content = StatusMessage;
									if (ChatListView.IsValid())
									{
										ChatListView->RequestListRefresh();
										ChatListView->RequestScrollIntoView(ProgressMessage);
									}
									LastUIUpdateTime = CurrentTime;
								}
								
								// 每10秒最多显示一条详细进度消息，避免刷屏
								if (StatusMessage != LastMessageContent || (CurrentTime - LastMessageTime) > 10.0f)
								{
									// 创建一个下载状态的单独消息 - 限制频率
									AddMessageToChat(TEXT("下载状态"), StatusMessage);
									
									// 更新上次消息内容和时间
									LastMessageContent = StatusMessage;
									LastMessageTime = CurrentTime;
								}
							}
						}
					}
					else
					{
						ConsecutiveEmptyReads++;
					}
				}
			}
			
			// 检测卡住的下载或加载过程(超过2分钟没有新输出)
			if (ProgressCounter > 3600 && ConsecutiveEmptyReads > 600) // 30帧 * 60秒 * 2分钟 = 3600
			{
				// 如果过了2分钟还没完成，可能卡住了
				if (ChatMessages.Contains(ProgressMessage) && !bHasDisplayedSuccess)
				{
					ChatMessages.Remove(ProgressMessage);
					
					if (bIsDownloading)
					{
						AddMessageToChat(TEXT("System"), FString::Printf(
							TEXT("下载模型 %s 时间过长。可能原因:"), *ModelName), true);
						AddMessageToChat(TEXT("System"), TEXT("1. 网络连接不稳定或速度慢"), true);
						AddMessageToChat(TEXT("System"), TEXT("2. 模型很大，需要更长时间"), true);
					}
					else
					{
						AddMessageToChat(TEXT("System"), FString::Printf(
							TEXT("加载模型 %s 时间过长。可能原因:"), *ModelName), true);
						AddMessageToChat(TEXT("System"), TEXT("1. 系统资源不足"), true);
						AddMessageToChat(TEXT("System"), TEXT("2. 模型损坏或不兼容"), true);
					}
					
					// 关闭管道
					if (ModelPipeRead != nullptr)
					{
						FPlatformProcess::ClosePipe(ModelPipeRead, nullptr);
					}
					
					// 重置服务启动标志和首次运行标志，允许再次点击启动按钮
					static_cast<FAI_ChangingModule*>(FModuleManager::Get().GetModule("AI_Changing"))->ResetServiceStartingFlag();
					bFirstRun = true;
					
					// 移除此委托（一次性使用）
					FCoreDelegates::OnEndFrame.RemoveAll(this);
					return;
				}
			}
		});
	}
}

void FAI_ChangingModule::RegisterPeriodicCheck()
{
	// 初始化时间计数
	AIChangingTimerState::CheckCount = 0;
	AIChangingTimerState::LastCheckTime = FPlatformTime::Seconds();
	
	// 添加一个更新委托，这在所有UE版本中都可用
	FCoreDelegates::OnEndFrame.AddLambda([this]()
	{
		double CurrentTime = FPlatformTime::Seconds();
		
		// 每5秒检查一次
		if (CurrentTime - AIChangingTimerState::LastCheckTime > 5.0 && AIChangingTimerState::CheckCount < 5)
		{
			AIChangingTimerState::LastCheckTime = CurrentTime;
			
			UE_LOG(LogTemp, Warning, TEXT("AI聊天插件命令检查 %d: %s"), 
				AIChangingTimerState::CheckCount,
				(PluginCommands.IsValid() && FAI_ChangingCommands::Get().OpenAIDialogCommand.IsValid()) ? TEXT("可用") : TEXT("不可用"));
			
			// 不再重复调用RegisterMenus，避免重复注册可能导致的问题
			// RegisterMenus();
			
			AIChangingTimerState::CheckCount++;
		}
	});
	
	UE_LOG(LogTemp, Warning, TEXT("已注册AI聊天插件周期性检查"));
}

// 清理所有Ollama进程
void FAI_ChangingModule::CleanupOllamaProcesses()
{
	UE_LOG(LogTemp, Warning, TEXT("正在关闭所有Ollama进程..."));
	
	// 标记所有进度监控都应该停止
	OllamaProgressMonitor::bShouldStopMonitoring = true;
	
#if PLATFORM_WINDOWS
	// 使用通用的终止命令停止所有ollama.exe进程
	FString KillCommand = TEXT("taskkill /F /IM ollama.exe");
	FPlatformProcess::ExecProcess(TEXT("cmd.exe"), *FString::Printf(TEXT("/c %s"), *KillCommand), nullptr, nullptr, nullptr, nullptr);
	UE_LOG(LogTemp, Warning, TEXT("已尝试终止所有Ollama进程"));
#else
	// 在非Windows平台上使用其他方法终止Ollama进程
	// ...
#endif
	
	// 重置服务启动状态
	bIsServiceStarting = false;
	
	// 5秒后重置标志，这样新的监控可以启动
	FTimerHandle TimerHandle;
	GWorld->GetTimerManager().SetTimer(TimerHandle, []()
	{
		OllamaProgressMonitor::bShouldStopMonitoring = false;
	}, 5.0f, false);
	
	UE_LOG(LogTemp, Warning, TEXT("Ollama进程清理完成"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAI_ChangingModule, AI_Changing)