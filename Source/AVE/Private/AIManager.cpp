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
	FVector SpawnPoint;
	/*for (int i = 0; i < spawnSwordmanCount; i++)
	{
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(GetWorld(), this->GetActorLocation(), SpawnPoint, spawnRadius, nullptr, nullptr);
		UE_LOG(LogTemp, Warning, TEXT("%f"), SpawnPoint.X);
		AEnemyBase* ab = GetWorld()->SpawnActor<AEnemyBase>(swordFactory, FTransform(FRotator(0), SpawnPoint, FVector(1)));

		ab->onSetManager(this);

		Enemies.AddUnique(ab);
	}*/
}

void AAIManager::StartAI()
{
	if (!running)
	{
		blackboard->SetValueAsObject(TEXT("PlayerActor"), UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		running = true;
		for (int i = 0; i < Enemies.Num() ; i++)
		{
			{
				int eNum = UAIBlueprintHelperLibrary::GetBlackboard(Enemies[i])->GetValueAsEnum(TEXT("AIState"));
				if(eNum != 3 || eNum != 4 || eNum != 5)
					UAIBlueprintHelperLibrary::GetBlackboard(Enemies[i])->SetValueAsEnum(TEXT("AIState"),1);
			}
		}
		return;
	}
	else
	return;
}