// Fill out your copyright notice in the Description page of Project Settings.


#include "SliceableActorBase.h"

// Sets default values
ASliceableActorBase::ASliceableActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASliceableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASliceableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

