// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AVEEnums.h"
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
	// 공격 판정을 계산할 무기 지정
	UFUNCTION(BlueprintCallable)
	void SetupWeapon(UStaticMeshComponent* WeaponMesh, float InWeaponThickness);

	// 공격 판정 계산
	void AttackCheckBegin();
	void AttackCheckTick();
	void AttackCheckEnd();

	// 가할 대미지량, 공격 타입 설정
	UFUNCTION(BlueprintCallable)
	void SetDamageInfo(float InBaseDamage, EDamageType InDamageType);

	// 가할 대미지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage;

	// 역경직 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitstopTime;

	// 대미지 피격 시 반응 (ex : 경직, 다운, 넉백, 공중에 뜸 등 )
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	EDamageType DamageType;

	// 자르기 가능하게 할지
	bool bEnableSlice = false;

	// 대미지 가하기
	void DealDamage(AActor* Target);

	// 역경직
	FTimerHandle HitstopTimer;
	void StartHitstop(float Time);
	void EndHitStop();

	// 무기 두께
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponThickness = 5.0f;

	// 무기 공격 판정 시작 지점
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName StartPoint = FName("WeaponStart");

	// 무기 공격 판정 끝 지점
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName EndPoint = FName("WeaponEnd");

	// 공격 판정을 계산할 무기
	class UStaticMeshComponent* MainWeapon;

	// 중복 공격 방지하기 위한 변수
	TArray<AActor*> AlreadyHitActors;

	// 타격 VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;
	void SpawnHitFX(FHitResult HitInfo);
};
