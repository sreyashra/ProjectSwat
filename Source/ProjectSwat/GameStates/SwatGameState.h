// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SwatGameState.generated.h"

class ASwatPlayerState;

UCLASS()
class PROJECTSWAT_API ASwatGameState : public AGameState
{
	GENERATED_BODY()

public:
	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated)
	TArray<ASwatPlayerState*> TopScoringPlayers;

	void UpdateTopScore(ASwatPlayerState* ScoringPlayer);

private:
	float TopScore = 0.f;
};
