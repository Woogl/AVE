// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "BaseBital.h"
#include "AVEEnums.h"
#include "EnemyBase.generated.h"

//추상 클래스에 구현할 공통 요소 
//ㄴ 체력, 공격, 피격, 가드, 회피 시스템 
//ㄴ 체간 : 다 차면 테이크다운 먹일 수 있음 
//ㄴ 사망 : 래그돌 

UCLASS()
class AVE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 플레이어 쳐다보기
	void LookAtPlayer();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/*virtual void ReceivePointDamage(float Damage, const UDamageType* DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo);*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float damage;		//공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hp;			//체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hpMax;		//최대체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posture;		//체간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureMax;	//최대체간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isGuard = false;	//가드 상태

	// 마무리 공격 당할 때
	UFUNCTION(BlueprintCallable)
		void OnFinishered();
	// 신체 절단
	UFUNCTION(BlueprintCallable)
		void SliceBodyPart(EBodyPart BodyIndex, FVector Impulse, float RagdollDelay);
	// 래그돌 활성화
	UFUNCTION(BlueprintCallable)
		void ActivateRagdoll();
	UFUNCTION(BlueprintCallable)
		void UpdateMoveSpeed(float NewSpeed);

	// 래그돌 활성화 타이머
	FTimerHandle RagdollTimer;
	//체간 회복 타이머
	FTimerHandle regenTimerHandle;
	//체간 회복 초기 딜레이
	float postureCool;
	//체간 회복 간격
	float postureRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* bodyMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		class UStaticMeshComponent* Weapon;
	UPROPERTY(EditAnywhere)
		class UAnimMontage* enemyAttackMontage;
	UPROPERTY(EditAnywhere)
		class UAnimMontage* enemyHitMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class UCombatComponent* CombatComp;
	UPROPERTY()
		class AAIManager* myManager;
	UPROPERTY(EditAnywhere)
		class UBaseBital* bital;
	UPROPERTY(EditAnywhere)
		class UBlackboardComponent* blackboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		class UWidgetComponent* enemyWidget;

	UFUNCTION(BlueprintCallable)
	virtual void onActionAttack();
	UFUNCTION(BlueprintCallable)
	virtual void onActionGuard();
	UFUNCTION(BlueprintCallable)
	virtual void onHit();
	UFUNCTION(BlueprintCallable)
	virtual void onHitCrushed();
	UFUNCTION(BlueprintCallable)
	virtual void onDie();	
	UFUNCTION(BlueprintCallable)
	virtual void onGetSet();
	UFUNCTION(BlueprintCallable)
	virtual void onSetManager(AAIManager* Manager);
	void regenPosture();

	UFUNCTION(BlueprintCallable)
	float GetHP();
	UFUNCTION(BlueprintCallable)
	float GetMaxHP();
	UFUNCTION(BlueprintCallable)
	float GetPosture();
	UFUNCTION(BlueprintCallable)
	float GetMaxPosture();
};