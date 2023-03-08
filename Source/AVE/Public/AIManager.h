// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.h"
#include "AC_Manager.h"
#include "SpawnPointer.h"
#include "AIManager.generated.h"

UCLASS()
class AVE_API AAIManager : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int AttackChance;
	UPROPERTY()
		class UBlackboardComponent* blackboard;
	bool running;


	UPROPERTY(EditAnywhere)
		int spawnSwordmanCount;
	UPROPERTY(EditAnywhere)
		int spawnGunmanCount;
	UPROPERTY(EditAnywhere)
		int spawnShielderCount;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AEnemyBase> swordFactory;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AEnemyBase> gunFactory;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AEnemyBase> shielderFactory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> SpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> Enemies;
	UPROPERTY()
		class APawn* PlayerCharacter;

	UFUNCTION(BlueprintCallable)
		void RunAI();
	void EnemyDelete(AActor* const InPawn);
	UFUNCTION(BlueprintCallable)
		void EnemySpawn(TArray<FVector> Points);
	UFUNCTION(BlueprintCallable)
		void EnemySearch();
};