// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HotelGenerator.generated.h"

UCLASS()
class AVE_API AHotelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AHotelGenerator();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* Root;

	// 건물 외관
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* Floor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* WallF;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* WallB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* WallL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* WallR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* PillarFR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* PillarFL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* PillarBR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* PillarBL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* DoorF;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	UStaticMeshComponent* DoorB;

	// 건물 내관
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop")
	TSubclassOf<AActor> InnerDoor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop")
	TArray<TSubclassOf<AActor>> SmallProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop")
	TArray<TSubclassOf<AActor>> LargeProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop")
	TArray<TSubclassOf<AActor>> WallProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Prop")
	TArray<TSubclassOf<AActor>> CeilingProps;

protected:
	virtual void BeginPlay() override;

public:	
	// 메쉬 기본값
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FVector FloorMesh = FVector(400.f, 400.f, 20.f);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	float FloorMeshY = 400.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FVector WallMesh = FVector(400.f, 20.f, 400.f);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	float PillarMeshZ = 400.f;

	// 호텔 사이즈 정하기
	UFUNCTION(BlueprintCallable)
	void SetHotelSize(float InSizeX, float InSizeY, int InTotalFloors);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorSizeX = 3000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorSizeY = 3000.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorHeightZ = 500.f;	// 층 높이는 기본값 그대로
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	int32 NumOfFloors = 5;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float UnitSize = 200.f;

	// 사이즈 조정
	float FloorScaleX = FloorSizeX / FloorMesh.X;
	float FloorScaleY = FloorSizeY / FloorMeshY;
	float WallScaleZ = FloorHeightZ / WallMesh.Z;
	float PillarScaleZ = FloorHeightZ / PillarMeshZ;
	FVector HotelScale = FVector(FloorScaleX, FloorScaleY, WallScaleZ);

	// 그리드 나누기
	int GridNumX;
	int GridNumY;
	void CreateDebugGrid();

	// 방 나누기
	float SplitChance = 0.5f;
	void TryCreatePartition();
	bool CanSplit();
	void SeparateHorizontal();
	void SeparateVertical();

	// 건물 세우기
	void MakeOuterWall();
	void MakeInnerWall();

	// TODO: 프롭 배치하기 (바닥, 벽, 천장)
	// TODO: 꼭대기 층만 천장 추가하기
};
