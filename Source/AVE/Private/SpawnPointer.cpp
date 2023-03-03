// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPointer.h"

// Sets default values
ASpawnPointer::ASpawnPointer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawnPointer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnPointer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int ASpawnPointer::GetSelector()
{
	return this->spawnSelector;
}