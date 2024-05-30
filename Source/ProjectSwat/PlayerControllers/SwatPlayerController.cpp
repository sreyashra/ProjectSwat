// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ProjectSwat/Characters/SwatCharacter.h"
#include "ProjectSwat/GameModes/SwatGameMode.h"
#include "ProjectSwat/GameStates/SwatGameState.h"
#include "ProjectSwat/HUD/Announcement.h"
#include "ProjectSwat/HUD/SwatHUD.h"
#include "ProjectSwat/HUD/CharacterOverlay.h"
#include "ProjectSwat/PlayerStates/SwatPlayerState.h"
#include "ProjectSwat/SwatComponents/CombatComponent.h"

void ASwatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SwatHUD = Cast<ASwatHUD>(GetHUD());
	ServerCheckMatchState();
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

void ASwatPlayerController::ServerCheckMatchState_Implementation()
{
	if (ASwatGameMode* GameMode = Cast<ASwatGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ASwatPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	MatchState = StateOfMatch;
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	OnMatchStateSet(MatchState);

	if (SwatHUD && MatchState == MatchState::WaitingToStart)
	{
		SwatHUD->AddAnnouncement();
	}
}

void ASwatPlayerController::SetHUDTime()
{
	float TimeLeft = 0;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	/*if (HasAuthority())
	{
		SwatGameMode = SwatGameMode == nullptr ? Cast<ASwatGameMode>(UGameplayStatics::GetGameMode(this)) : SwatGameMode;
		if (SwatGameMode)
		{
			SecondsLeft = FMath::CeilToInt(SwatGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}*/
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
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
		if (CooldownTime < 0)
		{
			SwatHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		SwatHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ASwatPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;

	if (bool bHUDValid = SwatHUD && SwatHUD->Announcement && SwatHUD->Announcement->WarmupTime)
	{
		if (CountdownTime < 0)
		{
			SwatHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime/60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		SwatHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
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
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ASwatPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
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

void ASwatPlayerController::HandleCooldown()
{
	SwatHUD = SwatHUD == nullptr ? Cast<ASwatHUD>(GetHUD()) : SwatHUD;
	if (SwatHUD)
	{
		SwatHUD->CharacterOverlay->RemoveFromParent();
		if (SwatHUD->Announcement && SwatHUD->Announcement->AnnouncementText && SwatHUD->Announcement->InfoText)
		{
			SwatHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match starts in:");
			SwatHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ASwatGameState* SwatGameState = Cast<ASwatGameState>(UGameplayStatics::GetGameState(this));
			ASwatPlayerState* SwatPlayerState = GetPlayerState<ASwatPlayerState>();
			if (SwatGameState && SwatPlayerState)
			{
				TArray<ASwatPlayerState*> TopPlayers = SwatGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == SwatPlayerState)
				{
					InfoTextString = FString("You are the winner.");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win: \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				SwatHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	ASwatCharacter* SwatCharacter = Cast<ASwatCharacter>(GetPawn());
	if (SwatCharacter && SwatCharacter->GetCombatComponent())
	{
		SwatCharacter->bDisableGameplay = true;
		SwatCharacter->GetCombatComponent()->FirePressed(false);
	}
}
