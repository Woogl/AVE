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

	// ���� ĳ����
	ACharacter* Grabber;
	USkeletalMeshComponent* GrabberMesh;

	// true�� ���� ����� ����
	bool ShouldAttack = false;
	
	// �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	EDamageType DamageType = EDamageType::EDT_Standard;

	// Ÿ�� VFX, SFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class UNiagaraSystem* HitNiagara;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundWave* HitSound;

	UStaticMeshComponent* GetMesh();


	//


	// �ֿ����� �� ȣ��
	void OnGrabbed(ACharacter* InGrabber);
	void AttachToGrabber();
	FTimerHandle AttachTimer;

	// �������� �� ȣ��
	void OnThrown(FVector ThrowingLoc);
	void ResetCollisionChannel();
	FTimerHandle ThrowTimer;

	// �������� �� ȣ��
	void OnDiscard();

	// �浹 �� ȣ��
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
