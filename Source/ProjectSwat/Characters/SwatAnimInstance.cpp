// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatAnimInstance.h"
#include "CharacterTrajectoryComponent.h"
#include "SwatCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void USwatAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SwatCharacter = Cast<ASwatCharacter>(TryGetPawnOwner());

	if (SwatCharacter != nullptr)
	{
		TrajectoryComponent = SwatCharacter->FindComponentByClass<UCharacterTrajectoryComponent>();
	}
}

void USwatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (SwatCharacter == nullptr)
	{
		SwatCharacter = Cast<ASwatCharacter>(TryGetPawnOwner());
	}

	if (SwatCharacter != nullptr)
	{
		FVector Velocity = SwatCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		bIsInAir = SwatCharacter->GetCharacterMovement()->IsFalling();

		bIsAccelerating = SwatCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
	}
}
