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

	// �ǹ� �ܰ�
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

	// �ǹ� ���׸���
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
	// ���� ġ�� ����
	UFUNCTION(BlueprintCallable)
	void SetupDimension(FVector InDimension = FVector(3000.f, 3000.f, 500.f));
	UPROPERTY(BlueprintReadWrite, Category = "Custom", meta = (ExposeOnSpawn = "true"))
	FVector Dimension;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float GridUnit = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	int CurFloor = -1;

	// �޽� ����
	FVector FloorOrigin;
	FVector FloorExtent;
	FVector WallExtent;
	FVector vNA;
	float fNA;

	// �׸��� ������
	int GridNumX;
	int GridNumY;
	UFUNCTION(BlueprintCallable)
	void CreateDebugGrid();

	// �� ������
	float SplitChance = 0.5f;
	void TryCreatePartition();
	bool CanSeparate();
	void SeparateHorizontal();
	void SeparateVertical();

	// �ǹ� �����
	UFUNCTION(BlueprintCallable)
	void MakeExteriors();
	UFUNCTION(BlueprintCallable)
	void MakeInteriors();
	void DestroyInteriors();

	// TODO: ���� ��ġ�ϱ� (�ٴ�, ��, õ��)
	// TODO: ����� ���� õ�� �߰��ϱ�
};
