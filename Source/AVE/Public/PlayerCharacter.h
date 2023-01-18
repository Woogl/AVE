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
	// ���� ������ �����ϴ� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCombatComponent* CombatComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCharacterMovementComponent* MoveComp;
	// Ÿ���� ���
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyTarget = nullptr;
	// Ÿ���� ��������� ȸ�� �ӵ� ( ȸ�� �Ϸ���� 1/x ��. 0�̸� ��� ȸ�� �Ϸ� )
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RInterpSpeed = 0.f;

	// ���� ����
	bool bIsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// ���� ��
	bool bIsBlocking = false;
	bool bIsDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTargeting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// ���� ��
	bool bIsParrying = false;
	FTimerHandle ParryingTimer;

	// ü��
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// ���� ��
	float CurHealth;

	// ü��
	float MaxPosture = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	// ���� ��
	float CurPosture;

	// �ִϸ��̼� ��Ÿ��
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
	TArray<class UAnimMontage*> GuardHitMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	class UAnimMontage* ParryingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Guards")
	class UAnimMontage* GuardBreakMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Dodges")
	TArray<class UAnimMontage*> DodgeMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	TArray<class UAnimMontage*> InteractionMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	TArray<class UAnimMontage*> HitReactionMontages;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �� �Է�
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// �Է� Ŀ�ǵ� ó��
	void CreateMove(const char NewInput);
	void CreateAttack(FString InputCommands);
	void CreateDash();

	// �׼� �Է�
	void Jump();
	void Guard();
	void StopGuard();
	void Interact();
	void Dash();
	void StopDash();
	void Finisher();

	// ���� üũ
	bool CanJump();
	bool CanAttack();
	bool CanGuard();
	bool CanInteract();
	bool CanDash();

	// ���� ���� �ε巴�� ȸ��
	void RotateToTarget(AActor* Target, float DeltaTime, float InterpSpeed);

	// ���� ���
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation);

	// ���
	void OnParryEnd();

	// ȸ��
	void PerformDodge();

	// ������ ����
	UFUNCTION(BlueprintCallable)
	void FinishEnemy();

	// ��� ���� (BP���� �̺�Ʈ ����)
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorph();

	// ī�޶� ��ȯ
	void MoveCamera(ECameraPosition CameraPosition);

	// �޽� �ڸ��� ���� ����
	void SpawnMeshSlicer();

	// ���� Ÿ����
	bool TryAutoTargeting();

public:
	// Ŀ�ǵ� ���
	TArray<FVector2D> MoveCommands;
	int Tail = -1;
	// Ŀ�ǵ� ���� Ÿ�̸�
	float LastMoveTime;
	float MoveResetLimit = 0.4f;

	// ���� �޺�
	int Combo = -1;
	// �޺� ���� Ÿ�̸�
	float LastAttackTime;
	float ComboResetLimit = 0.6f;

	// ���� Ư������ �ε���
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

};
