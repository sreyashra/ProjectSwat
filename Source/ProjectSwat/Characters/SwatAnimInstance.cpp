// Fill out your copyright notice in the Description page of Project Settings.


#include "SwatAnimInstance.h"
#include "CharacterTrajectoryComponent.h"
#include "SwatCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectSwat/Weapons/Weapon.h"

DEFINE_LOG_CATEGORY(LogSwatAnimInstance);

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
		
		bWeaponEquipped = SwatCharacter->IsWeaponEquipped();
		EquippedWeapon = SwatCharacter->GetEquippedWeapon();

		bIsCrouched = SwatCharacter->bIsCrouched;

		bAiming = SwatCharacter->IsAiming();

		TurningInPlace = SwatCharacter->GetTurningInPlace();

		// Yaw offset for strafing
		FRotator AimRotation = SwatCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(SwatCharacter->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
		YawOffset = DeltaRotation.Yaw;

		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = SwatCharacter->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw/DeltaSeconds;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
		Lean = FMath::Clamp(Interp, -90.f, 90.f);

		AO_Yaw = SwatCharacter->GetAO_Yaw();
		AO_Pitch = SwatCharacter->GetAO_Pitch();

		if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && SwatCharacter->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			SwatCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));
		}
	}
}
