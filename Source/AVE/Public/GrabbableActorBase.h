// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AVEEnums.h"
#include "GrabbableActorBase.generated.h"

UCLASS()
class AVE_API AGrabbableActorBase : public AActor
{
	GENERATED_BODY()

public:
	AGrabbableActorBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UNiagaraComponent* ElectricArc;

protected:
	virtual void BeginPlay() override;

public:
	// true�� ���� ����� ����
	bool bShouldAttack = false;

	// �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float BaseDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<class UAVEDamageType> DamageType;

	// VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
		class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
		class USoundWave* HitSound;

	// ����� �� ȣ��
	void OnGrabbed();
	FTimerHandle ElectricArcTimer;
	void StartElectricArc();
	void EndElectricArc();

	// �浹 �� ȣ��
	UFUNCTION()
		void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// ī���� �ߵ�
	UFUNCTION(BlueprintImplementableEvent)
		void FractureMesh();

	UStaticMeshComponent* GetMesh() const;
};
