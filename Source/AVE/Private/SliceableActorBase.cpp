// Fill out your copyright notice in the Description page of Project Settings.


#include "SliceableActorBase.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

ASliceableActorBase::ASliceableActorBase()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	OriginalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OriginalMesh"));
	OriginalMesh->SetCollisionProfileName(TEXT("NoCollision"));
	OriginalMesh->SetHiddenInGame(true);
	OriginalMesh->SetupAttachment(RootComponent);

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->bUseComplexAsSimpleCollision = false;
	ProceduralMesh->SetSimulatePhysics(true);
	ProceduralMesh->SetCollisionProfileName(TEXT("Destructible"));
	ProceduralMesh->SetupAttachment(RootComponent);
}

void ASliceableActorBase::BeginPlay()
{
	Super::BeginPlay();

	// OriginalMesh를 복사하여 ProceduralMesh에 보관
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(OriginalMesh, 0, ProceduralMesh, true);
}
