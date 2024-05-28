// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatPlayerState.h"
#include "ProjectSwat/Characters/SwatCharacter.h"
#include "ProjectSwat/PlayerControllers/SwatPlayerController.h"
#include "Net/UnrealNetwork.h"

void ASwatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASwatPlayerState, Defeats);
}

void ASwatPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	SwatCharacter = SwatCharacter == nullptr ? Cast<ASwatCharacter>(GetPawn()) : SwatCharacter;
	if (SwatCharacter)
	{
		SwatPlayerController = SwatPlayerController == nullptr ? Cast<ASwatPlayerController>(SwatCharacter->GetController()) : SwatPlayerController;
		if (SwatPlayerController)
		{
			SwatPlayerController->SetHUDScore(GetScore());
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
			SwatPlayerController->SetHUDScore(GetScore());
		}
	}
}

void ASwatPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;

	SwatCharacter = SwatCharacter == nullptr ? Cast<ASwatCharacter>(GetPawn()) : SwatCharacter;
	if (SwatCharacter)
	{
		SwatPlayerController = SwatPlayerController == nullptr ? Cast<ASwatPlayerController>(SwatCharacter->GetController()) : SwatPlayerController;
		if (SwatPlayerController)
		{
			SwatPlayerController->SetHUDDefeats(Defeats);
		}
	}
}

void ASwatPlayerState::OnRep_Defeats()
{
	SwatCharacter = SwatCharacter == nullptr ? Cast<ASwatCharacter>(GetPawn()) : SwatCharacter;
	if (SwatCharacter)
	{
		SwatPlayerController = SwatPlayerController == nullptr ? Cast<ASwatPlayerController>(SwatCharacter->GetController()) : SwatPlayerController;
		if (SwatPlayerController)
		{
			SwatPlayerController->SetHUDDefeats(Defeats);
		}
	}
}
