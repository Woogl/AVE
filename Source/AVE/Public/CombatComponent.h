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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AVE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 공격 판정을 계산할 무기
	UPROPERTY()
	class UMeshComponent* MainWeapon;
	UFUNCTION(BlueprintCallable)
		void SetupWeapon(UMeshComponent* WeaponMesh);

	// 공격 판정 계산
	UFUNCTION()
	void AttackCheckBegin();
	UFUNCTION()
	void AttackCheckTick();

	// 대미지
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

	// 역경직
	UPROPERTY()
	float HitstopTime;
	UFUNCTION()
	void StartHitstop(float Time);
	UFUNCTION()
	void EndHitStop();
	FTimerHandle HitstopTimer;

	// 자르기 가능하게 할지
	bool bEnableSlice = false;

	// 중복 공격 방지
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	// 트레이스
	UPROPERTY()
	TArray<FName> SocketNames;
	UPROPERTY()
	TArray<FVector> CurSocketLocations;
	UPROPERTY()
	TArray<FVector> LastSocketLocations;
	// 트레이스 채널 기본값 = EnemyAttack
	// NOTE : 주인공은 PlayerCharacter::BeginPlay()에서 트레이스 채널을 PlayerAttack으로 바꿔줬음
	UPROPERTY()
	TEnumAsByte<ETraceTypeQuery> AttackTrace = TraceTypeQuery5;

	// 타격 VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	UFUNCTION()
	void PlayHitFX(FHitResult HitInfo);

	// 메시 자르는 액터 스폰
	UFUNCTION()
	void SpawnMeshSlicer(FHitResult HitInfo);
};
