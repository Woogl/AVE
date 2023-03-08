// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include <Particles/ParticleSystem.h>
#include "EnemyShielder.generated.h"

/**
 * 
 */
UCLASS()
class AVE_API AEnemyShielder : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyShielder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Shield;

	UPROPERTY(EditAnywhere)
		USoundBase* fireSound;
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* bombingEffect;
	FTimerHandle ft;
	bool broken;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void onActionAttack() override;
	void onActionGuard() override;
	void onRemoveGuard();
	void onHit() override;
	void onHitCrushed() override;
	void onDie() override;
	void onGetSet() override;
	void onSuicide();
	UFUNCTION(BlueprintImplementableEvent)
		void onBeginBP();
	UFUNCTION(BlueprintImplementableEvent)
		void onHitBP(float damageAmount);
};
