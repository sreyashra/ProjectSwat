// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectSwat/Characters/SwatCharacter.h"
#include "ProjectSwat/PlayerControllers/SwatPlayerController.h"
#include "ProjectSwat/PlayerStates/SwatPlayerState.h"

void ASwatGameMode::PlayerEliminated(ASwatCharacter* ElimmedCharacter, ASwatPlayerController* VictimController,
                                     ASwatPlayerController* AttackerController)
{
	ASwatPlayerState* AttackerPlayerState = AttackerController ? Cast<ASwatPlayerState>(AttackerController->PlayerState) : nullptr;
	ASwatPlayerState* VictimPlayerState = VictimController ? Cast<ASwatPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ASwatGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(),PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
