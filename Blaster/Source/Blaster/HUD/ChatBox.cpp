// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBox.h"
#include "Components/EditableTextBox.h"
#include "ChatTextBlock.h"

void UChatBox::OnTextCommitted(const FText& Text, const FString& PlayerName)
{
    /*
     OwningController = OwningController == nullptr ? GetOwningPlayer() : OwningController;

    if (ChatTextBlockClass && OwningController)
    {
        BlasterPlayerState = BlasterPlayerState == nullptr ? OwningController->GetPlayerState<ABlasterPlayerState>() : BlasterPlayerState;
        if (BlasterPlayerState)
        {
            int32 LastIndex = ChatTextBlocks.Num();
            UChatTextBlock* ChatTextBlock = CreateWidget<UChatTextBlock>(GetOwningPlayer(), ChatTextBlockClass);
            if (ChatTextBlock && ChatTextBlock->ChatTextSlot)
            {
                const FString TextToDisplay = FString::Printf(TEXT("%s: %s"), *PlayerName, *Text.ToString());
                ChatTextBlock->ChatTextSlot->SetText(FText::FromString(TextToDisplay));
                ChatTextBlocks.Add(ChatTextBlock);
                ChatTextBox->AddChild(ChatTextBlock);
            }
        }
    }
    ChatInput->SetText(FText::FromString(""));
    */
    
}
