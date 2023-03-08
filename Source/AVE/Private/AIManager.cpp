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

	PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	blackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	EnemySearch();
	/*SpawnPoints.Add(FVector(310, 210, 310));
	SpawnPoints.Add(FVector(410, 210, 310));
	SpawnPoints.Add(FVector(510, 210, 310));
	SpawnPoints.Add(FVector(610, 210, 310));
	EnemySpawn(SpawnPoints);*/
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

void AAIManager::EnemySpawn(TArray<FVector> Points)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int i = 0; i < Points.Num(); i++)
	{
		if (i < spawnSwordmanCount)
		{
			AEnemyBase* swordman = GetWorld()->SpawnActor<AEnemyBase>(swordFactory, Points[i], FRotator::ZeroRotator, SpawnInfo);
			Enemies.AddUnique(swordman);
			swordman->onSetManager(this);
			continue;
		}

		// Spawn Gunman characters at current spawn point
		else if (i < spawnGunmanCount + spawnSwordmanCount)
		{
			AEnemyBase* Gunman = GetWorld()->SpawnActor<AEnemyBase>(gunFactory, Points[i], FRotator::ZeroRotator, SpawnInfo);
			//Enemies.AddUnique(Gunman);
			Gunman->onSetManager(this);
			continue;
		}

		// Spawn Shielder characters at current spawn point
		else if (i < spawnShielderCount + spawnGunmanCount + spawnSwordmanCount)
		{
			AEnemyBase* Shielder = GetWorld()->SpawnActor<AEnemyBase>(shielderFactory, Points[i], FRotator::ZeroRotator, SpawnInfo);
			Enemies.AddUnique(Shielder);
			Shielder->onSetManager(this);
			continue;
		}
	}
}

void AAIManager::EnemySearch()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);
	for (AActor* enemy : Enemies)
	{
		Cast<AEnemyBase>(enemy)->onSetManager(this);
		Enemies.AddUnique(Cast<AEnemyBase>(enemy));
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
				Cast<AEnemyBase>(Enemies[i])->onGetSet();
			}
		}
		return;
	}
	else
		return;
}

void AAIManager::EnemyDelete(AActor* const InPawn)
{
	Enemies.Remove(InPawn);
	if (Enemies.IsEmpty())
		EnemySpawn(SpawnPoints);
		//this->Destroy();
}