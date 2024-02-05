// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnToMainMenu.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Blaster/Character/BlasterCharacter.h"

//
#include "Kismet/GameplayStatics.h"

void UReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	TObjectPtr<UWorld> World = GetWorld();
	if (World) 
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;

		if (PlayerController)
		{
			FInputModeGameAndUI InputModeGame;
			InputModeGame.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeGame);
			PlayerController->SetShowMouseCursor(true);

		}
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	TObjectPtr<UGameInstance> GameInstance = GetGameInstance();
	
	if (GameInstance) 
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		if (MultiplayerSessionsSubsystem)
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
		}
	}
}

void UReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();

	TObjectPtr<UWorld> World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;

		if (PlayerController)
		{
			FInputModeGameOnly InputModeGame;
			PlayerController->SetInputMode(InputModeGame);
			PlayerController->SetShowMouseCursor(false);

		}
	}
	
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
	}
}

bool UReturnToMainMenu::Initialize()
{
	if (!Super::Initialize()) 
	{
		return false;
	}

	return true;
}

void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful) 
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}

	TObjectPtr<UWorld> World = GetWorld();
	
	if (World) 
	{
		TObjectPtr<AGameModeBase> GameMode = World->GetAuthGameMode<AGameModeBase>();

		if (GameMode)
		{
			//
			// Iterate through all connected players and disconnect them
			TArray<AActor*> Characters;
			UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Characters);	
			for (AActor* Character : Characters) 
			{
				TObjectPtr<ABlasterCharacter> BlasterCharacter = Cast<ABlasterCharacter>(Character);
				if (BlasterCharacter) 
				{
					PlayerController = BlasterCharacter->GetController<APlayerController>();

					
					if (PlayerController && !PlayerController->HasAuthority()) // && !PlayerController->HasAuthority()
					{
						if (!PlayerController->HasAuthority()) 
						{
							PlayerController->ClientReturnToMainMenuWithTextReason(FText());
						}
					}
				}
			}
			
			GameMode->ReturnToMainMenuHost();
		}
		else 
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;

			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);

	TObjectPtr<UWorld> World = GetWorld();

	if (World) 
	{
		TObjectPtr<APlayerController> FirstPlayerController = World->GetFirstPlayerController();

		if (FirstPlayerController) 
		{
			TObjectPtr<ABlasterCharacter> BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (BlasterCharacter)
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else 
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}

}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}