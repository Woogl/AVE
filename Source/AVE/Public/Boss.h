// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossFSMComponent.h"
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

	UPROPERTY(EditAnywhere)
	class UBossFSMComponent* bossFSMComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBossAnimInstance* asBossAnim;

	UPROPERTY()
	class APawn* playerPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnR180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnR90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnL180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnL90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animDashATK01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animDashATK02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animNormalATK01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animNormalATK02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animJumpATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animStanceToLowATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animStanceToStab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBehindATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBackstep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBladeAura;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnMyPlayMontageSA(UAnimMontage* selectMontage, EBossState selectState);
	UFUNCTION(BlueprintImplementableEvent)
	void OnMyPlayMontageNO(UAnimMontage* selectMontage, EBossState selectState);
	
	UPROPERTY(EditAnywhere)
	float currentHP;
	UPROPERTY(EditAnywhere)
	float maxHP = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSuperArmor;

	float MontageLength;
	float distanceValue;
	
	UFUNCTION(BlueprintCallable)
	void AnimTurnInPlace();
	UFUNCTION(BlueprintCallable)
	void SetFocusPlayerTick();

	void SetZeroSpeed();
	float DistanceBossToPlayer();
};
