// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ProjectSwat/PlayerControllers/SwatPlayerController.h"
#include "SwatGameMode.generated.h"

class ASwatCharacter;
class ASwatPlayerController;

UCLASS()
class PROJECTSWAT_API ASwatGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASwatGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(ASwatCharacter* ElimmedCharacter, ASwatPlayerController* VictimController, ASwatPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;
	
};
