// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DeveloperSettings.h"
#include "AI_ChangingSettings.generated.h"

/**
 * Settings for the AI_Changing plugin
 */
UCLASS(config=EditorPerProjectUserSettings, meta=(DisplayName="AI Chat Settings"))
class AI_CHANGING_API UAI_ChangingSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UAI_ChangingSettings();
	
	// The base URL of the AI API server
	UPROPERTY(config, EditAnywhere, Category="API", meta=(DisplayName="AI Server URL"))
	FString AIServerURL;
	
	// Path to send chat completion requests
	UPROPERTY(config, EditAnywhere, Category="API", meta=(DisplayName="Chat Completions Endpoint"))
	FString ChatCompletionsEndpoint;
	
	// Get the full URL for the chat completions endpoint
	FString GetChatCompletionsURL() const;
	
	// UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;
	// End of UDeveloperSettings interface
}; 