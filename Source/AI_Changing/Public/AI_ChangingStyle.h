// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Defines the visual style for the AI_Changing plugin UI
 */
class FAI_ChangingStyle
{
public:
	static void Initialize();
	static void Shutdown();

	/** @return The Slate style set for the AI plugin */
	static TSharedRef<FSlateStyleSet> Get();

	/** @return The name of our style set */
	static FName GetStyleSetName();
	
	/** Creates the style set */
	static TSharedRef<FSlateStyleSet> Create();

private:
	static TSharedPtr<FSlateStyleSet> StyleInstance;
}; 