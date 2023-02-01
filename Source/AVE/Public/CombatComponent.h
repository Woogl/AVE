// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AVEEnums.h"
#include "AllAVEDamageTypes.h"
#include "CombatComponent.generated.h"

/**
* 플레이어와 적의 공격 판정을 구하는 컴포넌트
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
	// 공격 판정을 계산할 무기
	class UMeshComponent* MainWeapon;
	UFUNCTION(BlueprintCallable)
	void SetupWeapon(UMeshComponent* WeaponMesh);

	// 공격 판정 계산
	void AttackCheckBegin();
	void AttackCheckTick();

	// 대미지
	float BaseDamage;
	TSubclassOf<UAVEDamageType> DamageType;
	void SetDamageInfo(float InBaseDamage, TSubclassOf<UDamageType> InDamageType);
	void OnAttackSucceed(TArray<FHitResult> Hits);
	void DealDamage(AActor* Target);

	// 역경직
	float HitstopTime;
	void StartHitstop(float Time);
	void EndHitStop();
	FTimerHandle HitstopTimer;

	// 자르기 가능하게 할지
	bool bEnableSlice = false;

	// 중복 공격 방지
	TArray<AActor*> AlreadyHitActors;

	// 트레이스
	TArray<FName> SocketNames;
	TArray<FVector> CurSocketLocations;
	TArray<FVector> LastSocketLocations;
	TArray< AActor* > ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	// 타격 VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	void PlayHitFX(FHitResult HitInfo);

	// 메시 자르는 액터 스폰
	void SpawnMeshSlicer(FHitResult HitInfo);
};
