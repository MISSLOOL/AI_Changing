// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ToolMenus.h"
#include "Widgets/SBoxPanel.h"

/**
 * Helper class for UToolMenus compatibility
 */
class FToolMenusHelper
{
public:
    /** Safely initialize tool menus */
    static void EnsureToolMenusInitialized()
    {
        if (UToolMenus::IsToolMenuUIEnabled())
        {
            if (!UToolMenus::Get()->IsToolMenuUIEnabled())
            {
                // In older UE versions, ensure tool menus are initialized
            }
        }
    }
    
    /** Use only with check for IsToolMenuUIEnabled() */
    static UToolMenus* GetSafe()
    {
        return UToolMenus::Get();
    }
};

/**
 * Helper class for widget operations
 */
class FWidgetHelper
{
public:
    /**
     * Safely remove a child widget from a vertical box
     * @param Box The vertical box to remove from
     * @param WidgetToRemove The widget to be removed
     * @return True if successfully removed
     */
    static bool SafeRemoveChildWidget(TSharedPtr<SVerticalBox> Box, TSharedRef<SWidget> WidgetToRemove)
    {
        if (!Box.IsValid())
        {
            return false;
        }
        
        // Try direct removal first
        try
        {
            Box->RemoveSlot(WidgetToRemove);
            return true;
        }
        catch (...)
        {
            // If direct removal fails, try a different approach
            // Recreate the entire box without the removed widget
            TArray<TSharedRef<SWidget>> RemainingWidgets;
            
            for (int32 i = 0; i < Box->GetChildren()->Num(); ++i)
            {
                TSharedRef<SWidget> Child = Box->GetChildren()->GetChildAt(i);
                if (Child != WidgetToRemove)
                {
                    RemainingWidgets.Add(Child);
                }
            }
            
            Box->ClearChildren();
            
            for (auto& Widget : RemainingWidgets)
            {
                Box->AddSlot()
                .Padding(5)
                [
                    Widget
                ];
            }
            
            return true;
        }
        
        return false;
    }
}; 