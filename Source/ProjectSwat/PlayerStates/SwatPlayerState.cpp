// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatPlayerState.h"
#include "ProjectSwat/Characters/SwatCharacter.h"
#include "ProjectSwat/PlayerControllers/SwatPlayerController.h"

void ASwatPlayerState::AddToScore(float ScoreAmount)
{
	Score +=ScoreAmount;

	SwatCharacter = SwatCharacter == nullptr ? Cast<ASwatCharacter>(GetPawn()) : SwatCharacter;
	if (SwatCharacter)
	{
		SwatPlayerController = SwatPlayerController == nullptr ? Cast<ASwatPlayerController>(SwatCharacter->GetController()) : SwatPlayerController;
		if (SwatPlayerController)
		{
			SwatPlayerController->SetHUDScore(Score);
		}
	}
}

void ASwatPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	SwatCharacter = SwatCharacter == nullptr ? Cast<ASwatCharacter>(GetPawn()) : SwatCharacter;
	if (SwatCharacter)
	{
		SwatPlayerController = SwatPlayerController == nullptr ? Cast<ASwatPlayerController>(SwatCharacter->GetController()) : SwatPlayerController;
		if (SwatPlayerController)
		{
			SwatPlayerController->SetHUDScore(Score);
		}
	}
}
