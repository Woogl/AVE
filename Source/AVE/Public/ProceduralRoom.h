// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoom.generated.h"

UCLASS()
class AVE_API AProceduralRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	AProceduralRoom();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* FloorMesh;

	// 배치할 사물
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PropClass;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 사물 배치하기
	void SpawnItem(UClass* ItemToSpawn);
	float RadiusForSpawn = 25.f;

	// 구획 나누기
	float RoomX = 1000.f;
	float RoomY = 1000.f;
	float RoomZ = 0.f;

	FVector TopLeft;
	FVector BottomRight = TopLeft + FVector(RoomX, RoomY, RoomZ);

	int32 GridNumX = 5;
	int32 GridNumY = 5;

	float SquareWidth = 200.f;
	float SquareHeight = 1.f;

	void CreateDebugGrid();

	// 위치 찾기
	FVector GetRandomPointInSquare(const FVector& UpperLeft, const FVector& LowerRight);
	void PlacePointsOnGrid();
};
