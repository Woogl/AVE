// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AVEEnums.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class AVE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
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
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	float TurnRateGamepad;
	// 공격 판정을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCombatComponent* CombatComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCharacterMovementComponent* MoveComp;
	// 타게팅 대상
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyTarget = nullptr;
	// 회전 속도 ( 회전 완료까지 1/x 초. 0이면 즉시 회전 완료 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RInterpSpeed = 0.f;

	// 상태 변수
	bool bIsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlocking = false;
	bool bIsDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTargeting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsParrying = false;
	bool bGuardBroken;
	bool bIsDead;
	FTimerHandle ParryingTimer;
	bool bIsGrabbing = false;

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurHealth = MaxHealth;

	// 체간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPosture = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurPosture = MaxPosture;

	// 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> ComboAttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> SpecialAttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	class UAnimMontage* JumpAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	class UAnimMontage* DashAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Finishers")
	TArray<class UAnimMontage*> FinisherMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	TArray<class UAnimMontage*> ParryHitMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	TArray<class UAnimMontage*> GuardHitMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	class UAnimMontage* GuardBreakMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Dodges")
	TArray<class UAnimMontage*> DodgeMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	TArray<class UAnimMontage*> InteractionMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	TArray<class UAnimMontage*> HitReactionMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	class UAnimMontage* DieMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	class UAnimMontage* GrabMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	class UAnimMontage* ThrowMontage;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 축 입력
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// 입력 커맨드 처리
	void CreateMove(const char NewInput);
	void CreateAttack(FString InputCommands);
	void CreateDash();

	// 액션 입력
	void Jump();
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

	// 부드럽게 회전
	void RotateToDirection(FVector Direction, float DeltaTime, float InterpSpeed);

	// 각도 계산
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation);

	// 방어
	void OnParryEnd();

	// 회피
	void PerformDodge();

	// 마무리 공격
	UFUNCTION(BlueprintCallable)
	void FinishEnemy();

	// 물건 줍기, 던지기
	void TryGrab();
	void TryThrow();
	void PerformThrow();
	class AGrabbableActorBase* GrabbedObject;

	// 모션 워핑 (BP에서 이벤트 구현)
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorph();

	// 카메라 전환
	void MoveCamera(ECameraPosition CameraPosition);

	// 메시 자르는 액터 스폰
	void SpawnMeshSlicer();

	// 오토 타게팅
	UFUNCTION(BlueprintCallable)	// 연구 중
	bool TryAutoTargeting(float SearchRadius = 300.f);

	// 잔상 생성
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnGhostTrail();

public:
	// 커맨드 어레이
	TArray<FVector2D> MoveCommands;
	int Tail = -1;
	// 커맨드 리셋 타이머
	float LastMoveTime;
	float MoveResetLimit = 0.4f;

	// 현재 콤보
	int Combo = -1;
	// 콤보 리셋 타이머
	float LastAttackTime;
	float ComboResetLimit = 1.f;

	// 현재 특수공격 인덱스
	int SpecialAttackIndex = 0;

	void WInput();
	void SInput();
	void DInput();
	void AInput();

	void CreateMoveCommand(FVector2D InputDirection);

	void Attack();

	void JumpAttack();
	void SpecialAttack();
	void DashAttack();
	void ComboAttack();

	void InitState();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ParryHit(float Damage, int DamageType);
	void GuardHit(float Damage, int DamageType);
	void Hit(float Damage, int DamageType);
	void GuardBreak();
	void Die();
};
