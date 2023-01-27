// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hp = 2;
	posture = 1;
	damage = 1;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

}
 
// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemyBase::onActionAttack()
{
}

void AEnemyBase::onActionEvade()
{
}

void AEnemyBase::onActionGuard()
{
}

void AEnemyBase::onHit(int characterDamage)
{
	hp -= characterDamage;
	if (hp < 0)
		onDie();
}

void AEnemyBase::onHitCrushed()
{
}

void AEnemyBase::onDie()
{
	Destroy();
}