// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatBox.generated.h"


class UScrollBox;
class UEditableTextBox;
class ABlasterPlayerState;
class UChatTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UChatBox : public UUserWidget
{
	GENERATED_BODY()

public:

	void OnTextCommitted(const FText& Text, const FString& PlayerName);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInput;

	//UPROPERTY(EditAnywhere)
	//TSubclassOf<UChatTextBlock> ChatTextBlockClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<UChatTextBlock*> ChatTextBlocks;

private:

	UPROPERTY()
	TObjectPtr<APlayerController> OwningController;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerState> BlasterPlayerState;
};
