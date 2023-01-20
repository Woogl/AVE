// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AVEEnums.h"
#include "CombatComponent.generated.h"

/**
* �÷��̾�� ���� ���� ������ ���ϴ� ������Ʈ
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AVE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	// ���� ������ ����� ���� ����
	UFUNCTION(BlueprintCallable)
	void SetupWeapon(UStaticMeshComponent* WeaponMesh, float InWeaponThickness);

	// ���� ���� ���
	void AttackCheckBegin();
	void AttackCheckTick();
	void AttackCheckEnd();

	// ���� �������, ���� Ÿ�� ����
	UFUNCTION(BlueprintCallable)
	void SetDamageInfo(float InBaseDamage, EDamageType InDamageType);

	// ���� �����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage;

	// ������ �ð�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitstopTime;

	// ����� �ǰ� �� ���� (ex : ����, �ٿ�, �˹�, ���߿� �� �� )
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	EDamageType DamageType;

	// �ڸ��� �����ϰ� ����
	bool bEnableSlice = false;

	// ����� ���ϱ�
	void DealDamage(AActor* Target);

	// ������
	FTimerHandle HitstopTimer;
	void StartHitstop(float Time);
	void EndHitStop();

	// ���� �β�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponThickness = 5.0f;

	// ���� ���� ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName StartPoint = FName("WeaponStart");

	// ���� ���� ���� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName EndPoint = FName("WeaponEnd");

	// ���� ������ ����� ����
	class UStaticMeshComponent* MainWeapon;

	// �ߺ� ���� �����ϱ� ���� ����
	TArray<AActor*> AlreadyHitActors;

	// Ÿ�� VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	void SpawnHitFX(FHitResult HitInfo);
};
