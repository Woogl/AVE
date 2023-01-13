// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EAttackType.h"
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
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ���� ������ ����� ���� ����
	UFUNCTION(BlueprintCallable)
	void SetupWeapon(UStaticMeshComponent* WeaponMesh, float InWeaponThickness);

	// ���� ���� ���
	void AttackCheckBegin();
	void AttackCheckTick();
	void AttackCheckEnd();

	// ���� �������, ���� Ÿ�� ����
	UFUNCTION(BlueprintCallable)
	void SetDamageInfo(float InBaseDamage, EAttackType InAttackType);

	// ���� �����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Custom")
	float BaseDamage;

	// ������ �ð�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Custom")
	float HitstopTime;

	// ����� �ǰ� �� ���� (ex : ����, �ٿ�, �˹�, ���߿� �� �� )
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Custom")
	EAttackType AttackType;

	// �ڸ��� �����ϰ� ����
	bool bEnableSlice = false;

	// ����� ���ϱ�
	void DealDamage(AActor* Target);

	// ������
	FTimerHandle HitstopTimer;
	void StartHitstop(float Time);
	void EndHitStop();

	// ���� �β�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Please")
	float WeaponThickness = 5.0f;

	// ���� ���� ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Please")
	FName StartPoint = FName("WeaponStart");

	// ���� ���� ���� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Please")
	FName EndPoint = FName("WeaponEnd");

	// ���� ������ ����� ����
	class UStaticMeshComponent* MainWeapon;

	// �ߺ� ���� �����ϱ� ���� ����
	TArray<AActor*> AlreadyHitActors;
};
