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
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	ASwatHUD* SwatHUD;
	
};
