// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();
	
	TObjectPtr<UGameInstance> GameInstance = GetGameInstance();

	if (GameInstance)
	{	
		TObjectPtr<UMultiplayerSessionsSubsystem> Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		check(Subsystem);

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			TObjectPtr<UWorld> World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;
				FString MatchType = Subsystem->DesiredMatchType;
				
				if (MatchType == "FreeForAll")
				{
					World->ServerTravel(FString("/Game/Maps/Blaster?listen"));
				}
				else if (MatchType == "Teams")
				{
					World->ServerTravel(FString("/Game/Maps/BlasterTeams?listen"));
				}
				else if (MatchType == "CaptureTheFlag")
				{
					World->ServerTravel(FString("/Game/Maps/BlasterCTF?listen"));
				}
			}
		}
	}
}
