// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HotelFloor.generated.h"

UCLASS()
class AVE_API AHotelFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHotelFloor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* Root;

	// 건물 외관
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* Floor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* WallF;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* WallB;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* WallL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* WallR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* PillarFR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* PillarFL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* PillarBR;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* PillarBL;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* DoorF;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exterior")
	UStaticMeshComponent* DoorB;

	// 건물 인테리어
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interior")
	TSubclassOf<AActor> InnerDoor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interior")
	TArray<TSubclassOf<AActor>> SmallProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interior")
	TArray<TSubclassOf<AActor>> LargeProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interior")
	TArray<TSubclassOf<AActor>> WallProps;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interior")
	TArray<TSubclassOf<AActor>> CeilingProps;

protected:
	virtual void BeginPlay() override;

public:	
	// 층의 치수 설정
	UFUNCTION(BlueprintCallable)
	void SetupDimension(FVector InDimension = FVector(3000.f, 3000.f, 500.f));
	UPROPERTY(BlueprintReadWrite, Category = "Custom", meta = (ExposeOnSpawn = "true"))
	FVector Dimension;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float GridUnit = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	int CurFloor = -1;

	// 메쉬 정보
	FVector FloorOrigin;
	FVector FloorExtent;
	FVector WallExtent;
	FVector vNA;
	float fNA;

	// 그리드 나누기
	int GridNumX;
	int GridNumY;
	UFUNCTION(BlueprintCallable)
	void CreateDebugGrid();

	// 방 나누기
	float SplitChance = 0.5f;
	void TryCreatePartition();
	bool CanSeparate();
	void SeparateHorizontal();
	void SeparateVertical();

	// 건물 세우기
	UFUNCTION(BlueprintCallable)
	void MakeExteriors();
	UFUNCTION(BlueprintCallable)
	void MakeInteriors();
	void DestroyInteriors();

	// TODO: 프롭 배치하기 (바닥, 벽, 천장)
	// TODO: 꼭대기 층만 천장 추가하기
};
