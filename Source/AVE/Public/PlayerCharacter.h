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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* DefaultCameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USceneComponent* GrabPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* Scabbard;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USpotLightComponent* CharLighting;
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
	// ȸ�� �ӵ� ( ȸ�� �Ϸ���� 1/x ��. 0�̸� ��� ȸ�� �Ϸ� )
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RInterpSpeed = 0.f;

	// ���� ����
	bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	bool bIsBlocking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTargeting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	bool bIsParrying;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDashing;
	// ����극��ũ ����
	bool bIsGuardBroken;
	// ��ų ���� �� ���� ����
	bool bIsInvincible;
	// �ǰ� ����(�и���Ʈ, ������Ʈ, �븻��Ʈ ���� X), ������ ���� ����
	bool bIsHit;
	// ���� ���� ����
	bool bIsLightningCharged;
	bool bIsDead;
	FTimerHandle ParryingTimer;
	bool bIsGrabbing = false;

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurHealth = MaxHealth;

	// ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPosture = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurPosture = MaxPosture;

	// �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> ComboAttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> SpecialAttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> SkillMontages;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Attacks")
	TArray<class UAnimMontage*> JumpAttackMontages;
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
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Interactions")
	class UAnimMontage* GanpaMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	class UAnimMontage* ChargeMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	class UAnimMontage* GroggyMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | HitReactions")
	class UAnimMontage* DieMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds | Footstep")
	class USoundBase* WetFootstep;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds | Footstep")
	class USoundBase* DryFootstep;

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

	// �ε巴�� ȸ��
	void RotateToDirection(FVector Direction, float DeltaTime = 0.f, float InterpSpeed = 0.f);

	// ���� ���
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation);

	// ���
	void OnParryEnd();

	// ȸ��
	void PerformDodge();

	// ������ ����
	UFUNCTION(BlueprintCallable)
	void FinishEnemy();

	// ���� �ݱ�, ������
	void PullProp();
	void PushProp();
	void DropProp();
	class AGrabbableActorBase* GrabbedActor;
	UFUNCTION()
	void AttachGrabbedActor();

	// ��� ���� (BP���� �̺�Ʈ ����)
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorph();
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorphGanpa(); // ���� ��� ���� ���ĺ��� 
	UFUNCTION(BlueprintImplementableEvent)
	void MotionMorphGrabHit(); // ���� ��� ���� �ǰ�

	// ���� Ÿ����
	UFUNCTION(BlueprintCallable)	// ���� ��
	bool TryAutoTargeting(float SearchRadius = 300.f);

	// �ܻ� ����
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnGhostTrail();

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
	float ComboResetLimit = 1.f;

	// ���� Ư������ �ε���
	UPROPERTY(BlueprintReadOnly)
	int CurSpecialAttack;
	UPROPERTY(BlueprintReadOnly)
	int CurSkill;
	UPROPERTY(BlueprintReadOnly)
	int CurKatasiro = 5;
	UPROPERTY(BlueprintReadOnly)
	float SkillCooltime = 5.f;
	UPROPERTY(BlueprintReadOnly)
	float SpecialAttackCooltime = 5.f;
	// ���� ����
	float Defense;
	// ���� ����
	float LightningDefense;
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
	void InitInvincibility();
	void InitCharge();
	void InitGuard();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ParryHit(float Damage, TSubclassOf<UDamageType> DamageType);
	void GuardHit(float Damage, TSubclassOf<UDamageType> DamageType);
	void Hit(float Damage, TSubclassOf<UDamageType> DamageType);
	void GuardBreak();
	void Groggy();
	void Charge();
	void Die();

	void Skill();
	void ChangeSkill();
	void ChangeSpecialAttack();
	UFUNCTION(BlueprintCallable)
	void MoveWeaponLeft();
	UFUNCTION(BlueprintCallable)
	void MoveWeaponRight();
	UFUNCTION(BlueprintCallable)
	void SpreadAoEDamage(TSubclassOf<UDamageType> AttackDamageType);

	void RegeneratePosture();

	// ó�� ������ ����
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFinisherSequence();
	// ���� ������ ����
	UFUNCTION(BlueprintImplementableEvent)
	void PlayLightningShockSequence();
	// �ݰ��� ������ ����
	UFUNCTION(BlueprintImplementableEvent)
	void PlayJudgementCutSequence();
	// ��ų 1 ������ ����
	UFUNCTION(BlueprintImplementableEvent)
	void PlayEarthquakeSequence();
	// ����ź ������ ����
	UFUNCTION(BlueprintImplementableEvent)
	void PlayMissileSequence();

	void PlayWetFootstepSound(FVector Location);
	void PlayDryFootstepSound(FVector Location); 
};
