// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectSwat/ProjectSwatTypes/TurningInPlace.h"
#include "SwatAnimInstance.generated.h"

class ASwatCharacter;
class UCharacterTrajectoryComponent;
class AWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogSwatAnimInstance, Log, All);

UCLASS()
class PROJECTSWAT_API USwatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = "true"))
	ASwatCharacter* SwatCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Trajectory, meta=(AllowPrivateAccess = "true"))
	UCharacterTrajectoryComponent* TrajectoryComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta=(AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Combat, meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float AO_Pitch;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bLocallyControlled;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bRotateRootBone;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bElimmed;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bUseFABRIK;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bUseAimOffsets;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bTransformRightHand;
};
