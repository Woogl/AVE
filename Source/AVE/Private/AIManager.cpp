// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "EnemySwordman.h"
#include "Containers/Array.h"
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <Runtime/NavigationSystem/Public/NavigationSystem.h>
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AAIManager::AAIManager()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAC_Manager::StaticClass();

	running = false;
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();
	EnemySpawn();

	PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	blackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
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
	for (int i = 0; i < SpawnPoints.Num(); i++)
	{
		// Spawn Swordman characters at current spawn point
		if (i < spawnSwordmanCount)
		{
			AEnemyBase* Swordman = GetWorld()->SpawnActor<AEnemyBase>(swordFactory, SpawnPoints[i]->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Swordman);
			Swordman->onSetManager(this);
		}

		// Spawn Gunman characters at current spawn point
		else if (i < spawnGunmanCount + spawnSwordmanCount)
		{
			AEnemyBase* Gunman = GetWorld()->SpawnActor<AEnemyBase>(gunFactory, SpawnPoints[i]->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Gunman);
			Gunman->onSetManager(this);
		}

		// Spawn Shielder characters at current spawn point
		else if (i < spawnShielderCount + spawnGunmanCount + spawnSwordmanCount)
		{
			AEnemyBase* Shielder = GetWorld()->SpawnActor<AEnemyBase>(shielderFactory, SpawnPoints[i]->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Shielder);
			Shielder->onSetManager(this);
		}
	}
}

void AAIManager::RunAI()
{
	if (!running)
	{
		blackboard->SetValueAsObject(TEXT("PlayerActor"), UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		running = true;
		for (int i = 0; i < Enemies.Num(); i++)
		{
			{
				int eNum = UAIBlueprintHelperLibrary::GetBlackboard(Enemies[i])->GetValueAsEnum(TEXT("AIState"));
				if (eNum != 3 || eNum != 4 || eNum != 5)
					UAIBlueprintHelperLibrary::GetBlackboard(Enemies[i])->SetValueAsEnum(TEXT("AIState"), 1);
				/*UAIBlueprintHelperLibrary::GetBlackboard(Enemies[i])->SetValueAsObject(TEXT("PlayerActor"), PlayerCharacter);*/
				Enemies[i]->onGetSet();
			}
		}
		return;
	}
	else
		return;
}

void AAIManager::EnemyDelete(AEnemyBase* const InPawn)
{
	Enemies.Remove(InPawn);
	if (Enemies.IsEmpty())
		this->Destroy();
}