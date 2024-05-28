// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SwatPlayerController.generated.h"

class ASwatHUD;

UCLASS()
class PROJECTSWAT_API ASwatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;
	
private:
	ASwatHUD* SwatHUD;
	
};
