// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UCLASS()
class AVE_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* weaponMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* weaponMeshSubComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCombatComponent* combatComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)\
	class UBossFSMComponent* bossFSMComp;

	UPROPERTY()
	class APawn* playerPawn;
	
	UPROPERTY()
	class UBossAnimInstance* asBossAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* bossTurnR180;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* bossTurnR90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* bossTurnL180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* bossTurnL90;
	
	UPROPERTY(EditAnywhere)
	float currentHP;
	UPROPERTY(EditAnywhere)
	float maxHP = 500.f;

	void SetZeroSpeed();
	void SetFocusPlayerTick();
	void AnimTurnInPlace();
	void WalkToLocation();
};
