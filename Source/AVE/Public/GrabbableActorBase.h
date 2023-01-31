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

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 잡은 캐릭터
	ACharacter* Grabber;
	USkeletalMeshComponent* GrabberMesh;

	// true일 때만 대미지 입힘
	bool ShouldAttack = false;
	
	// 대미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	EDamageType DamageType = EDamageType::EDT_Standard;

	// 타격 VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;

	UStaticMeshComponent* GetMesh();


	//


	// 주워졌을 때 호출
	void OnGrabbed(ACharacter* InGrabber);
	void AttachToGrabber();
	FTimerHandle AttachTimer;

	// 던져졌을 때 호출
	void OnThrown(FVector ThrowingLoc);
	void ResetCollisionChannel();
	FTimerHandle ThrowTimer;

	// 버려졌을 때 호출
	void OnDiscard();

	// 충돌 시 호출
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
