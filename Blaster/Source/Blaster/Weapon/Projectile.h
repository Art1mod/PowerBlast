// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class ABlasterCharacter;
class UNiagaraSystem;
class UNiagaraComponent;


UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	AProjectile();

	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/*
	* Used with server-side rewind
	*/

	bool bUseServerSideRewind = false;

	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float BulletInitialSpeed = 15000.f;

	UPROPERTY(EditAnywhere)
	float RocketInitialSpeed = 3000.f;
	
	// Only set this for Grenades and Rockets 
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	// Doesn't matter for Grenades and Rockets 
	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

protected:
	
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnHitParticles();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnBloodParticles(FVector_NetQuantize HitLocation);

	void SpawnTrailSystem();
	void ExplodeDamage();
	

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticlesPlayer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSoundPlayer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 500.f;


private: 

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> Tracer;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerComponent;

	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterCharacter;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:	
	
	

};
