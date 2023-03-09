// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AVEEnums.h"
#include "AllAVEDamageTypes.h"
#include "CombatComponent.generated.h"

/**
* �÷��̾�� ���� ���� ������ ���ϴ� ������Ʈ
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AVE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ���� ������ ����� ����
	UPROPERTY()
	class UMeshComponent* MainWeapon;
	UFUNCTION(BlueprintCallable)
		void SetupWeapon(UMeshComponent* WeaponMesh);

	// ���� ���� ���
	UFUNCTION()
	void AttackCheckBegin();
	UFUNCTION()
	void AttackCheckTick();

	// �����
	UPROPERTY()
	float BaseDamage;
	UPROPERTY()
	TSubclassOf<UAVEDamageType> DamageType;
	UFUNCTION()
	void SetDamageInfo(float InBaseDamage, TSubclassOf<UDamageType> InDamageType);
	UFUNCTION()
	void OnAttackSucceed(FHitResult HitInfo);
	UFUNCTION()
	void DealDamage(AActor* Target);

	// ������
	UPROPERTY()
	float HitstopTime;
	UFUNCTION()
	void StartHitstop(float Time);
	UFUNCTION()
	void EndHitStop();
	FTimerHandle HitstopTimer;

	// �ڸ��� �����ϰ� ����
	bool bEnableSlice = false;

	// �ߺ� ���� ����
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	// Ʈ���̽�
	UPROPERTY()
	TArray<FName> SocketNames;
	UPROPERTY()
	TArray<FVector> CurSocketLocations;
	UPROPERTY()
	TArray<FVector> LastSocketLocations;
	// Ʈ���̽� ä�� �⺻�� = EnemyAttack
	// NOTE : ���ΰ��� PlayerCharacter::BeginPlay()���� Ʈ���̽� ä���� PlayerAttack���� �ٲ�����
	UPROPERTY()
	TEnumAsByte<ETraceTypeQuery> AttackTrace = TraceTypeQuery5;

	// Ÿ�� VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	UFUNCTION()
	void PlayHitFX(FHitResult HitInfo);

	// �޽� �ڸ��� ���� ����
	UFUNCTION()
	void SpawnMeshSlicer(FHitResult HitInfo);
};
