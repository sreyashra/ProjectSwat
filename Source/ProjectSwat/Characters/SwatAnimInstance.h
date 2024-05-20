// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SwatAnimInstance.generated.h"

class ASwatCharacter;
class UCharacterTrajectoryComponent;

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
};
