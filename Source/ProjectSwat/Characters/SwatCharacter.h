// SwatCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectSwat/ProjectSwatTypes/TurningInPlace.h"
#include "ProjectSwat/Interfaces/InteractWithCrosshairsInterface.h"
#include "ProjectSwat/ProjectSwatTypes/CombatState.h"
#include "SwatCharacter.generated.h"

// Forward declarations
class UCombatComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterTrajectoryComponent;
struct FInputActionValue;
class UWidgetComponent;
class AWeapon;
class UAnimMontage;
class ASwatPlayerController;
class ASwatPlayerState;

DECLARE_LOG_CATEGORY_EXTERN(LogSwatCharacter, Log, All);

UCLASS(config=Game)
class PROJECTSWAT_API ASwatCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ASwatCharacter();

	// Tick function
	virtual void Tick(float DeltaTime) override;

	// Input handling
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_ReplicatedMovement() override;
	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	// Initialization
	virtual void PostInitializeComponents() override;

	// Animation montages
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayHitReactMontage();

	// Elimination
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	// Weapon
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

protected:
	// Initialization
	virtual void BeginPlay() override;
	
	// Input actions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void Equip();
	void CrouchButtonPressed();
	void Aim();
	void StopAim();
	void Fire();
	void StopFire();
	void Reload();

	// Aiming and rotation
	void CalculateAO_Pitch();
	void AimOffset(float DeltaTime);
	void SimProxiesTurn();
	void TurnInPlace(float DeltaTime);
	void RotateInPlace(float DeltaTime);

	// Damage handling
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();

	// HUD and initialization
	void PollInit();

private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta=(AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta=(AllowPrivateAccess = "true"))
	UCharacterTrajectoryComponent* CharacterTrajectoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UCombatComponent* Combat;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	// Widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	// Weapon
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquip();

	// Aiming and rotation
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	// Animation montages
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	// Camera
	void HideCameraIfCharacterIsClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	// Rotation and movement
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastReplicatedMovement;
	float CalculateSpeed();

	// Health
	UPROPERTY(EditAnywhere, Category=PlayerStats)
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = PlayerStats)
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	// Elimination
	bool bElimmed = false;
	FTimerHandle ElimTimer;
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	// Player controller and state
	UPROPERTY()
	ASwatPlayerController* SwatPlayerController;

	UPROPERTY()
	ASwatPlayerState* SwatPlayerState;

public:
	// Weapon
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetEquippedWeapon();

	// Getters
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }

	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;
};