// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySwordman.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AEnemySwordman::AEnemySwordman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AEnemySwordman::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySwordman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemySwordman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemySwordman::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void AEnemySwordman::onActionAttack()
{
}

void AEnemySwordman::onActionEvade()
{
}

void AEnemySwordman::onActionGuard()
{
}

void AEnemySwordman::onHit(int characterDamage)
{
}

void AEnemySwordman::onHitCrushed()
{
}

void AEnemySwordman::onDie()
{
}

