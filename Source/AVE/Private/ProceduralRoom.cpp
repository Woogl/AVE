// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"
#include "FloorNode.h"
#include "ProceduralFloor.h"


AProceduralRoom::AProceduralRoom()
{
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	RootComponent = FloorMesh;
}

void AProceduralRoom::BeginPlay()
{
	Super::BeginPlay();
	
	//CreateDebugGrid();
	//PlacePointsOnGrid();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("floor node count %d"), FloorNode::GetFloorNodeCount()));

	// 구획 나누기
	TSharedPtr<ProceduralFloor> theFloor(new ProceduralFloor());
	theFloor->MakePartition();
	theFloor->DrawFloorNodes(GetWorld());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Number of Partitioned floor stack %d"), theFloor->GetPartitionedFloor().Num()));
}

void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralRoom::SpawnItem(UClass* ItemToSpawn)
{
	float xCoord = FMath::FRandRange(-500.f, 500.f);
	float yCoord = FMath::FRandRange(-500.f, 500.f);
	float randYaw = FMath::FRandRange(0.f, 360.f);

	FVector locToSpawn = FVector(xCoord, yCoord, 0.f);
	FRotator rotToSpawn = FRotator(0.f, randYaw, 0.f);
	GetWorld()->SpawnActor<AActor>(ItemToSpawn, locToSpawn, rotToSpawn);
}

void AProceduralRoom::CreateDebugGrid()
{
	// X축 구분선
	for (int32 i = 0; i < GridNumX + 1; i++)
	{
		FVector start = TopLeft + FVector(i * SquareWidth, 0.f, SquareHeight);
		FVector end = start + FVector(0.f, RoomX, SquareHeight);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	} 
	// Y축 구분선
	for (int32 i = 0; i < GridNumX + 1; i++)
	{
		FVector start = TopLeft + FVector(0.f, i * SquareWidth, SquareHeight);
		FVector end = start + FVector(RoomY, 0.f, SquareHeight);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	}
}

FVector AProceduralRoom::GetRandomPointInSquare(const FVector& UpperLeft, const FVector& LowerRight)
{
	float randomX = FMath::FRandRange(UpperLeft.X, LowerRight.X);
	float randomY = FMath::FRandRange(UpperLeft.Y, LowerRight.Y);
	return FVector(randomX, randomY, 0.f);
}

void AProceduralRoom::PlacePointsOnGrid()
{
	for (int32 i = 0; i < GridNumX; i++)
	{
		for (int32 j = 0; j < GridNumY; j++)
		{
			FVector upperLeft(i * SquareWidth + RadiusForSpawn, j * SquareWidth + RadiusForSpawn, SquareHeight);
			FVector lowerRight(i * SquareWidth + SquareWidth - RadiusForSpawn, j * SquareWidth + SquareWidth - RadiusForSpawn, SquareHeight);
			FVector randomPointInSquare = GetRandomPointInSquare(upperLeft, lowerRight);

			//DrawDebugCircle(GetWorld(), randomPointInSquare, RadiusForSpawn, 48, FColor::Red, true, -1.f, 0, 2.5f, FVector(0.f, 1.f, 0.f), FVector(1.f, 0.f, 0.f), true);
			
			float randomYaw = FMath::FRandRange(0.f, 360.f);
			GetWorld()->SpawnActor<AActor>(PropClass, randomPointInSquare, FRotator(0.f, randomYaw, 0.f));
		}
	}
}

