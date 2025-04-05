// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI_ChangingSettings.h"

UAI_ChangingSettings::UAI_ChangingSettings()
{
	// 修改为Ollama的API服务器URL和端点
	AIServerURL = TEXT("http://localhost:11434");
	ChatCompletionsEndpoint = TEXT("/api/chat");
}

FString UAI_ChangingSettings::GetChatCompletionsURL() const
{
	return AIServerURL + ChatCompletionsEndpoint;
}

FName UAI_ChangingSettings::GetCategoryName() const
{
	return FName("Plugins");
}

FText UAI_ChangingSettings::GetSectionText() const
{
	return FText::FromString("AI Chat");
} 