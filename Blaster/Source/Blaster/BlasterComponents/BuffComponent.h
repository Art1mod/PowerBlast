// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ABlasterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	friend class ABlasterCharacter;

	UBuffComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);

	void  BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	void SetInitialJumpVelocity(float Velocity);
	void BuffJump(float BuffJumpVelocity, float BuffTime);

protected:

	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

private:

	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;

	/*
	*	Heal buff
	*/

	bool bHealing = true;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	/*
	*	Shield buff
	*/
	
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0.f;
	float ShieldAmountToReplenish = 0.f;


	/*
	*	Speed buff
	*/
	
	FTimerHandle SpeedBuffTimer;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	/*
	* Jump buff
	*/

	FTimerHandle JumpBuffTimer;
	float InitialJumpVelocity;



	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	void ResetSpeeds();
	void ResetJump();


public:	
		
};
