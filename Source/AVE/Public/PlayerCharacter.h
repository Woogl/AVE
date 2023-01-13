// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ECameraPosition.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class AVE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* DefaultCameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* LeftCameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* RightCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Scabbard;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	float TurnRateGamepad;

	// 공격 판정을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCombatComponent* CombatComp;

	// 타게팅 대상
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyTarget = nullptr;

	// 축 입력
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// 액션 입력
	void Jump();
	void Attack();
	void Guard();
	void StopGuard();
	void Interact();
	void Dash();
	void StopDash();
	void Finisher();

	// 상태 체크
	bool CanJump();
	bool CanAttack();
	bool CanGuard();
	bool CanInteract();
	bool CanDash();

	// 적을 향해 부드럽게 회전
	void RotateToTarget(AActor* Target, float DeltaTime, float InterpSpeed);

	// 각도 계산
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation);

	// 오토 타게팅 
	bool TryAutoTargeting();
	AActor* GetNearestEnemy();

	// 공격
	void PerformLightAttack();
	void PerformJumpAttack();

	// 마무리 공격
	UFUNCTION(BlueprintCallable)
	void FinishEnemy();

	// 모션 워핑 (BP에서 이벤트 구현)
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorph();

	// 카메라 전환
	void MoveCamera(ECameraPosition CameraPosition);

	// 메시 자르는 액터 스폰
	void SpawnMeshSlicer();

private:
	// 무브셋
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> Attacks;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> JumpAttacks;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Finishers")
	TArray<UAnimMontage*> Finishers;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	TArray<UAnimMontage*> GuardHits;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	TArray<UAnimMontage*> GuardBreaks;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Dodges")
	TArray<class UAnimMontage*> Dodges;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	TArray<UAnimMontage*> Interactions;

	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	TArray<UAnimMontage*> HitReactions;

	// 상태 변수
	bool bIsAttacking = false;
	bool bIsBlocking = false;
	bool bIsDashing = false;
	bool bIsTargeting = false;

	// 공격 콤보 카운트
	int AttackCount = 0;
	int MaxAttackCount;

	// 적을 향한 부드러운 회전 속도 ( 회전 완료까지 1/x 초. 0이면 즉시 회전 완료 )
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RInterpSpeed = 0.f;

	/* 오토 타게팅 개선 중
	UFUNCTION(BlueprintCallable)
	bool NewTryAutoTargeting();
	AActor* SearchEnemies();
	void ScoreEnemies();
	void SetEnemyTarget(AActor* Target);
	void ClearScores();
	TArray<AActor*> SearchedEnemies;
	TArray<float> DistanceScores;
	TArray<float> AngleScores;
	TArray<float> TotalScores;
	*/

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return DefaultCameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
