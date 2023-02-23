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

	// 컴포넌트 및 캐스트 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* weaponMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* weaponMeshSubComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCombatComponent* combatComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBossFSMComponent* bossFSMComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBossAnimInstance* asBossAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerCharacter* asPlayer;
	UPROPERTY()
	class APawn* playerPawn;
	UPROPERTY()
	AActor* EnemyTarget = nullptr;

	// 턴 몽타주 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnR180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnR90;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnL180;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurnInPlace")
	class UAnimMontage* bossTurnL90;

	// 몽타주 애니메이션 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animDashATK01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animDashATK02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animNormalATKR01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animNormalATKL01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animJumpATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animStanceToLowATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animStanceToStab;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBehindATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBackStep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animBladeRangeATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animReboundR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animReboundL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animReboundATKR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animReboundATKL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animGrabATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animSlashATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animComboATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animStanceCounter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animParryR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animParryL;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	// class UAnimMontage* animParryRATK;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	// class UAnimMontage* animParryLATK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animWarCry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animInsal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	class UAnimMontage* animLaserRangeATK;

	// 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float currentHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxHP = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float bossPosture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float bossMaxPosture = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSuperArmor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanParry;
	
	
	FTimerHandle delayHandle;
	int randomIntValue;
	float randomFloatValue;
	float montageLength;
	int attackCount;
	int parryCount = 0;
	// float distanceValue;
	bool bTakeDamage = false;
	

	// 함수
	UFUNCTION()
	void AnimDashATK();
	UFUNCTION()
	void AnimNormalATK();
	UFUNCTION()
	void AnimJumpATK();
	UFUNCTION()
	void AnimStanceATK();
	UFUNCTION()
	void AnimBehindATK();
	UFUNCTION()
	void AnimBackStep();
	UFUNCTION()
	void AnimBladeRangeATK();
	UFUNCTION()
	void AnimGrabATK();
	UFUNCTION()
	void AnimSlashATK();
	UFUNCTION()
	void AnimComboATK();
	UFUNCTION()
	void AnimWarCry();
	UFUNCTION()
	void AnimInsal();
	UFUNCTION()
	void AnimLaserRangeATK();
	
	UFUNCTION()
	void ReturnToMove();
	UFUNCTION()
	void ReturnToWalk();
	UFUNCTION()
	void ReturnToBladeRangeATK();
	UFUNCTION()
	void RandomInt(int min, int max);
	UFUNCTION()
	void RandomFloat();
	UFUNCTION(BlueprintCallable)
	void AnimTurnInPlace();
	UFUNCTION(BlueprintCallable)
	void SetFocusPlayerInplace();
	UFUNCTION(BlueprintCallable)
	void SetFocusPlayerTick();
	UFUNCTION()
	void OnLineTraceHit();
	UFUNCTION()
	void TakeDamageFalse();
	UFUNCTION(BlueprintCallable)
	void ClearTags();
	
	void SetZeroSpeed();
	float DistanceBossToPlayer();
	void PostureRecovery();
	
	UFUNCTION()
	void AnimReboundATK();
	// UFUNCTION()
	// void AnimParryATK();
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	float bossArmor = 5.f;
};
