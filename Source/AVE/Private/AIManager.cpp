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

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPointer::StaticClass(), SpawnPoints);

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
	for (AActor* tempActor : SpawnPoints)
	{
		switch (Cast<ASpawnPointer>(tempActor)->GetSelector())
		{
		case 1:
		{
			AEnemyBase* Swordman = GetWorld()->SpawnActor<AEnemyBase>(swordFactory, tempActor->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Swordman);
			Swordman->onSetManager(this);
		}
		break;
		/*case 2:
		{
			AEnemyBase* Gunman = GetWorld()->SpawnActor<AEnemyBase>(gunFactory, tempActor->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Gunman);
			Gunman->onSetManager(this);
		}
		break;*/
		case 3:
		{
			AEnemyBase* Shielder = GetWorld()->SpawnActor<AEnemyBase>(shielderFactory, tempActor->GetActorLocation(), FRotator::ZeroRotator);
			Enemies.AddUnique(Shielder);
			Shielder->onSetManager(this);
		}
		break;
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

void AAIManager::InputPoint(AActor* const inActor)
{
	SpawnPoints.Add(inActor);
}