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

	// ���� ������ �����ϴ� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UCombatComponent* CombatComp;

	// Ÿ���� ���
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	AActor* EnemyTarget = nullptr;

	// �� �Է�
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// �׼� �Է�
	void Jump();
	void Attack();
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

	// ���� Ÿ���� 
	bool TryAutoTargeting();
	AActor* GetNearestEnemy();

	// ����
	void PerformLightAttack();
	void PerformJumpAttack();

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

private:
	// �����
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

	// ���� ����
	bool bIsAttacking = false;
	bool bIsBlocking = false;
	bool bIsDashing = false;
	bool bIsTargeting = false;

	// ���� �޺� ī��Ʈ
	int AttackCount = 0;
	int MaxAttackCount;

	// ���� ���� �ε巯�� ȸ�� �ӵ� ( ȸ�� �Ϸ���� 1/x ��. 0�̸� ��� ȸ�� �Ϸ� )
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RInterpSpeed = 0.f;

	/* ���� Ÿ���� ���� ��
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
