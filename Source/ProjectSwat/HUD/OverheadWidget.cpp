// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	APlayerController* PlayerController = Cast<APlayerController>(InPawn->GetController());
	APlayerState* PlayerState = PlayerController ? PlayerController->PlayerState : nullptr;
	FString PlayerName = PlayerState ? PlayerState->GetPlayerName() : "Unknown";
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
		
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("None");
		break;

	case ENetRole::ROLE_MAX:
		Role = FString("Max");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s, Name: %s"), *Role, *PlayerName);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
