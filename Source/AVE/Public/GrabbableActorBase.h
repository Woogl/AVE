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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* HighlightSphere;

protected:
	virtual void BeginPlay() override;

public:
	// true일 때만 대미지 입힘
	bool bShouldAttack = false;

	// 대미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<class UAVEDamageType> DamageType;

	// VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;

	// 잡았을 때 호출
	void OnGrabbed();
	FTimerHandle ElectricArcTimer;
	void StartElectricArc();
	void EndElectricArc();

	// 발사할 때 호출
	void OnPushed();

	// 떨굴 때 호출
	void OnDropped();

	// 충돌 시 호출
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// 외곽선 효과 키고 끄기
	UFUNCTION()
	void OnHighlightSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnHighlightSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 카오스 발동
	UFUNCTION(BlueprintImplementableEvent)
	void FractureMesh();

	UStaticMeshComponent* GetMesh() const;

	USkeletalMeshComponent* PlayerMesh;
};
