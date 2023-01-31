// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include <Runtime/NavigationSystem/Public/NavigationSystem.h>

// Sets default values
AAIManager::AAIManager()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAIManager::EnemySpawn()
{
	FVector SpawnPoint;
	for (int i = 0; i < spawnSwordmanCount; i++)
	{
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(GetWorld(), this->GetActorLocation(), SpawnPoint, spawnRadius, nullptr, nullptr);

		//GetWorld()->SpawnActor<ASwordman>(swordFactory, SpawnPoint);
	}
}
