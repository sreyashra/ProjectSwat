// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SwatPlayerState.generated.h"

class ASwatCharacter;
class ASwatPlayerController;

UCLASS()
class PROJECTSWAT_API ASwatPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);

private:
	ASwatCharacter* SwatCharacter;
	ASwatPlayerController* SwatPlayerController;
	
};
