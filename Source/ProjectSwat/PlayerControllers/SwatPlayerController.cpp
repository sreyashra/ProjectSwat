// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"
#include "ProjectSwat/Characters/SwatCharacter.h"
#include "ProjectSwat/HUD/Announcement.h"
#include "ProjectSwat/HUD/SwatHUD.h"
#include "ProjectSwat/HUD/CharacterOverlay.h"

void ASwatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SwatHUD = Cast<ASwatHUD>(GetHUD());
	if (SwatHUD)
	{
		SwatHUD->AddAnnouncement();
	}
}

void ASwatPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	PollInit();
}

void ASwatPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (SwatHUD && SwatHUD->CharacterOverlay)
		{
			CharacterOverlay = SwatHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void ASwatPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASwatPlayerController, MatchState);
}

void ASwatPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ASwatPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());

	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
}

void ASwatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASwatCharacter* SwatCharacter = Cast<ASwatCharacter>(InPawn))
	{
		SetHUDHealth(SwatCharacter->GetHealth(), SwatCharacter->GetMaxHealth());
	}
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ASwatPlayerController::SetHUDScore(float Score)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->ScoreAmount)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		SwatHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ASwatPlayerController::SetHUDDefeats(int32 Defeats)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->DefeatsAmount)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		SwatHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ASwatPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		SwatHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ASwatPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		SwatHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ASwatPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->CharacterOverlay && SwatHUD->CharacterOverlay->MatchCountdownText)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		SwatHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ASwatPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ASwatPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (RoundTripTime * 0.5f);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ASwatPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ASwatPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ASwatPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ASwatPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ASwatPlayerController::HandleMatchHasStarted()
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;
	if (SwatHUD)
	{
		SwatHUD->AddCharacterOverlay();
		if (SwatHUD->Announcement)
		{
			SwatHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}