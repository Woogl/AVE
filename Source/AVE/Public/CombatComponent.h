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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AVE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:	
	// ���� ������ ����� ����
	class UMeshComponent* MainWeapon;
	UFUNCTION(BlueprintCallable)
	void SetupWeapon(UMeshComponent* WeaponMesh);

	// ���� ���� ���
	void AttackCheckBegin();
	void AttackCheckTick();

	// �����
	float BaseDamage;
	TSubclassOf<UAVEDamageType> DamageType;
	void SetDamageInfo(float InBaseDamage, TSubclassOf<UDamageType> InDamageType);
	void OnAttackSucceed(TArray<FHitResult> Hits);
	void DealDamage(AActor* Target);

	// ������
	float HitstopTime;
	void StartHitstop(float Time);
	void EndHitStop();
	FTimerHandle HitstopTimer;

	// �ڸ��� �����ϰ� ����
	bool bEnableSlice = false;

	// �ߺ� ���� ����
	TArray<AActor*> AlreadyHitActors;

	// Ʈ���̽�
	TArray<FName> SocketNames;
	TArray<FVector> CurSocketLocations;
	TArray<FVector> LastSocketLocations;
	TArray< AActor* > ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	// Ÿ�� VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	void PlayHitFX(FHitResult HitInfo);

	// �޽� �ڸ��� ���� ����
	void SpawnMeshSlicer(FHitResult HitInfo);
};
