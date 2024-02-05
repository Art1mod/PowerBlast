// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent; 
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class USoundCue;
class ImpactParticles;
class ABlasterPlayerController;
class AController;
class ABlasterPlayerState;
class UBuffComponent;
class UBoxComponent;
class ULagCompensationComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class ABlasterGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:

	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PostInitializeComponents() override;

	/*
	* Play Montages
	*/

	void PlayFireMontage(bool bAiming);
	
	void PlayReloadMontage();
	
	void PlayElimMontage();
	
	void PlayThrowGrenadeMontage();
	
	void PlaySwapMontage();

	/*-----*/

	virtual void OnRep_ReplicatedMovement() override;

	void Elim(APlayerController* AttackerController, bool bPlayerLeftGame); 

	UFUNCTION(NetMulticast, Reliable) 
	void MulticastElim(const FString& AttackerName, bool bPlayerLeftGame);

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	
	void UpdateHUDShield();

	void UpdateHUDAmmo();
	
	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void  MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

protected:

	virtual void BeginPlay() override;

	/**Callbacks for inputs*/
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
	
	void EquipButtonPressed();
	
	void CrouchButtonPressed();

	void ReloadButtonPressed();

	void AimButtonPressed();

	void AimButtonReleased();

	void AimOffset(float DeltaTime);

	void CalculateAO_Pitch();

	void SimProxiesTurn();

	virtual void Jump() override;
	
	void FireButtonPressed();

	void FireButtonReleased();

	void PlayHitReactMontage();

	void GrenadeButtonPressed();

	void DropOrDestroyWeapon(AWeapon* Weapon);

	void DropOrDestroyWeapons();

	void SetSpawnPoint();

	void OnPlayerStateInitialized();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	void RotateInPlace(float DeltaTime);

	/*
	* Hit boxes used for server-side rewind 
	*/

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> head;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Pelvis;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_02;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> spine_03;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> UpperArm_L;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> UpperArm_R;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> lowerarm_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Hand_L;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Hand_R;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> backpack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> blanket;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Thigh_L;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Thigh_R;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_l;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> calf_r;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Foot_L;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Foot_R;

	/*----*/

private:


	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> FollowCamera;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/*
	* Blaster Components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBuffComponent> Buff;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULagCompensationComponent> LagCompensation;

	/*---*/

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/*
	*	Animation Montages
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> SwapMontage;

	/*---*/

	void HideCameraIfCharacterClose();

	float CameraThreshold = 200.f;

	bool bRotateRootBone;

	float TurnThreshold = 0.5f;

	FRotator ProxyRotationLastFrame;

	FRotator ProxyRotation;

	float ProxyYaw;

	float TimeSinceLastMovementReplication;

	float CalculateSpeed();

	/*
	* Player health
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/*---*/

	/*
	* Player shield
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;

	/*---*/

	bool bInputsSet;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> BlasterContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> UndoAimAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> UndoFireAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ThrowGrenadeAction;

	/*
	* Dissolve effect
	*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	TObjectPtr <UCurveFloat> DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstanceDynamic> DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance 
	UPROPERTY(VisibleAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	/*---*/

	/*
	*	Team colors
	*/

	UPROPERTY(EditAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> RedMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Elim)
	TObjectPtr<UMaterialInstance> BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* OriginalMaterial;

	/*
	* Elim effects
	*/

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> ElimBotComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ElimBotSound;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerState> BlasterPlayerState;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> CrownSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> CrownComponent;

	/*---*/

	/*
	* Grenade
	*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;

	/*---*/

	/*
	* Default weapon
	*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	void DefaultWeaponSpawnTimerFinished();

	FTimerHandle DefaultWeaponSpawnTimer;
	
	float DefaultWeaponSpawnDelay = 0.1f;

	/*---*/

	UPROPERTY()
	TObjectPtr<ABlasterGameMode> BlasterGameMode;


public:

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage()const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	FORCEINLINE bool IsHoldingTheFlag() const;
	ETeam GetTeam();
	void SetHoldingTheFlag(bool bHolding);
	
	AWeapon* GetEquippedWeapon();

};
