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

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor")
	TArray<TSubclassOf<class AHotelFloor>> FloorsToSpawn;

	// 호텔 치수 설정
	UFUNCTION(BlueprintCallable)
	void SetupHotel(FVector EachFloorDimension = FVector(3000.f, 3000.f, 500.f), int InTotalFloor = 5);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom")
	FVector FloorDimension;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Custom")
	int TotalFloor;

	// 호텔 짓기
	UFUNCTION(BlueprintCallable)
	void BuildHotel();
	UFUNCTION(BlueprintCallable)
	void GenerateFloor(FVector InFloorDimension, int CurFloor);
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnFloor();
};
