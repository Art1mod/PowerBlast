// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"


namespace MatchState 
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode() 
{
	bDelayedStart = true;
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::BeginPlay() 
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);

		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;

	TObjectPtr<ABlasterPlayerState> AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	TObjectPtr<ABlasterPlayerState> VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	TObjectPtr<ABlasterGameState> BlasterGameState = GetGameState<ABlasterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState) 
	{
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;

		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers) 
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);

		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState)) 
		{
			TObjectPtr<ABlasterCharacter> Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());

			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				TObjectPtr<ABlasterCharacter> Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				
				if (Loser) 
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	if (VictimPlayerState) 
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter) 
	{
		ElimmedCharacter->Elim(AttackerController, false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) 
	{
		TObjectPtr<ABlasterPlayerController> BlasterPlayer = Cast<ABlasterPlayerController>(*It);

		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}

}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		TObjectPtr<AActor> FurthestSpawn = FurthestSpawnFromAllCharacters();
		if (FurthestSpawn) 
		{
			RestartPlayerAtPlayerStart(ElimmedController, FurthestSpawn);
		}
		else 
		{
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

			if (PlayerStarts.Num() > 0)
			{
				int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
				RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found for respawn."));
			}
		}
		
	}
}

AActor* ABlasterGameMode::FurthestSpawnFromAllCharacters()
{
	/*
	TArray<AActor*>AllPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllPlayerStarts);

	TArray<AActor*>Characters;
	UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Characters);

	TArray<float>StockedDistances;

	for (int i = 0; i < AllPlayerStarts.Num(); i++)
	{
		float MinDistance = (AllPlayerStarts[i]->GetActorLocation() - Characters[0]->GetActorLocation()).Size();
		for (int j = 1; j < Characters.Num(); j++)
		{

			float Distance = (AllPlayerStarts[i]->GetActorLocation() - Characters[j]->GetActorLocation()).Size();
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
			}
		}
		StockedDistances.Add(MinDistance);
	}

	float MaxDistance = StockedDistances[0];
	int32 Selection = 0;

	for (int i = 1; i < StockedDistances.Num(); i++)
	{
		if (MaxDistance < StockedDistances[i])
		{
			MaxDistance = StockedDistances[i];
			Selection = i;
		}
	}

	return AllPlayerStarts[Selection];
	*/
	TArray<AActor*> AllPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllPlayerStarts);

	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Characters);

	if (AllPlayerStarts.Num() == 0 || Characters.Num() == 0)
	{
		// Handle the case where either AllPlayerStarts or Characters is empty
		return nullptr;
	}

	TArray<float> StockedDistances;

	for (AActor* PlayerStart : AllPlayerStarts)
	{
		if (Characters.Num() == 0)
		{
			// Handle the case where Characters is empty
			break;
		}

		float MinDistance = FVector::DistSquared(PlayerStart->GetActorLocation(), Characters[0]->GetActorLocation());

		for (int32 j = 1; j < Characters.Num(); j++)
		{
			float Distance = FVector::DistSquared(PlayerStart->GetActorLocation(), Characters[j]->GetActorLocation());

			if (Distance < MinDistance)
			{
				MinDistance = Distance;
			}
		}

		StockedDistances.Add(FMath::Sqrt(MinDistance));
	}

	if (StockedDistances.Num() == 0)
	{
		// Handle the case where StockedDistances is empty
		return nullptr;
	}

	float MaxDistance = StockedDistances[0];
	int32 Selection = 0;

	for (int32 i = 1; i < StockedDistances.Num(); i++)
	{
		if (MaxDistance < StockedDistances[i])
		{
			MaxDistance = StockedDistances[i];
			Selection = i;
		}
	}

	if (Selection < 0 || Selection >= AllPlayerStarts.Num())
	{
		// Handle the case where Selection is out of bounds
		return nullptr;
	}

	return AllPlayerStarts[Selection];

}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;

	TObjectPtr<ABlasterGameState> BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}

	TObjectPtr<ABlasterCharacter> CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());

	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(nullptr, true);
	}
}



