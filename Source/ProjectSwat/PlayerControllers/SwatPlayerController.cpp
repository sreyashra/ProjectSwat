// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ProjectSwat/HUD/SwatHUD.h"
#include "ProjectSwat/HUD/CharacterOverlay.h"

void ASwatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SwatHUD = Cast<ASwatHUD>(GetHUD());
}

void ASwatPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->HealthBar && SwatHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health / MaxHealth;
		SwatHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		SwatHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}