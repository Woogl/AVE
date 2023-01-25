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

	// ��ġ�� �繰
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PropClass;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �繰 ��ġ�ϱ�
	void SpawnItem(UClass* ItemToSpawn);
	float RadiusForSpawn = 25.f;

	// ��ȹ ������
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

	// ��ġ ã��
	FVector GetRandomPointInSquare(const FVector& UpperLeft, const FVector& LowerRight);
	void PlacePointsOnGrid();
};
