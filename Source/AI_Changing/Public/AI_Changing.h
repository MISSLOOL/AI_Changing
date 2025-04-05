// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "LevelEditor.h"
#include "Templates/SharedPointer.h"
#include "Framework/Commands/Commands.h"
#include "Http.h"
#include "AI_ChangingStyle.h"
#include "Widgets/Views/SListView.h"

class FToolBarBuilder;
class FMenuBuilder;
class SVerticalBox;
template<typename OptionType> class SComboBox;

// 聊天消息数据结构
struct FChatMessage
{
	FString Sender;
	FString Content;
	bool bIsError;
	
	FChatMessage(const FString& InSender, const FString& InContent, bool InIsError = false)
		: Sender(InSender), Content(InContent), bIsError(InIsError)
	{
	}
};

/** Command list declaration for the plugin */
class FAI_ChangingCommands : public TCommands<FAI_ChangingCommands>
{
public:
	FAI_ChangingCommands()
		: TCommands<FAI_ChangingCommands>(
			TEXT("AI_Changing"),
			NSLOCTEXT("Contexts", "AI_Changing", "AI_Changing Plugin"),
			NAME_None,
			TEXT("EditorStyle"))
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenAIDialogCommand;
};

class FAI_ChangingModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** Registers the AI chat tab spawner */
	void RegisterTabSpawner();
	
	/** Unregisters the AI chat tab spawner */
	void UnregisterTabSpawner();

	/** This function will be bound to Command. */
	void PluginButtonClicked();
	
	/** Called when the plugin tab is activated */
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
	
	/** Gets the list of available AI models */
	TArray<TSharedPtr<FString>> GetAvailableModels() const;
	
	/** Sends a request to the local AI model */
	void SendAIRequest(const FString& Prompt, const FString& SelectedModel);
	
	/** Registers toolbar extension */
	void RegisterMenus();
	
	/** Registers a periodic check for plugin state */
	void RegisterPeriodicCheck();
	
	/** 辅助方法：添加消息到聊天 */
	void AddMessageToChat(const FString& Sender, const FString& Message, bool bIsError = false);
	
	/** 启动Ollama服务和运行模型 */
	void StartOllamaService(const FString& ModelName);
	
	/** 重置服务启动状态标志 */
	void ResetServiceStartingFlag() { bIsServiceStarting = false; }
	
	/** 清理所有Ollama进程 */
	void CleanupOllamaProcesses();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TArray<TSharedPtr<FString>> AvailableModels;
	TArray<TSharedPtr<FString>> FilteredModels;  // 添加过滤后的模型列表
	TSharedPtr<SVerticalBox> ChatHistoryBox;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> ModelSelector;
	TSharedPtr<SBox> ModelSelectorContainer;  // 添加模型选择器容器
	TSharedPtr<class IHttpRequest, ESPMode::ThreadSafe> HTTPRequest;
	
	// 聊天消息列表和视图
	TArray<TSharedPtr<FChatMessage>> ChatMessages;
	TSharedPtr<SListView<TSharedPtr<FChatMessage>>> ChatListView;
	
	// 服务启动状态标志
	bool bIsServiceStarting = false;
};
