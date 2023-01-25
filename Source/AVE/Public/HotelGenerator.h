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

	// �ǹ� �ܰ�
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

	// �ǹ� ����
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
	// �޽� �⺻��
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FVector FloorMesh = FVector(400.f, 400.f, 20.f);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	float FloorMeshY = 400.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	FVector WallMesh = FVector(400.f, 20.f, 400.f);
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Default")
	float PillarMeshZ = 400.f;

	// ȣ�� ������ ���ϱ�
	UFUNCTION(BlueprintCallable)
	void SetHotelSize(float InSizeX, float InSizeY, int InTotalFloors);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorSizeX = 3000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorSizeY = 3000.f;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float FloorHeightZ = 500.f;	// �� ���̴� �⺻�� �״��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	int32 NumOfFloors = 5;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Custom")
	float UnitSize = 200.f;

	// ������ ����
	float FloorScaleX = FloorSizeX / FloorMesh.X;
	float FloorScaleY = FloorSizeY / FloorMeshY;
	float WallScaleZ = FloorHeightZ / WallMesh.Z;
	float PillarScaleZ = FloorHeightZ / PillarMeshZ;
	FVector HotelScale = FVector(FloorScaleX, FloorScaleY, WallScaleZ);

	// �׸��� ������
	int GridNumX;
	int GridNumY;
	void CreateDebugGrid();

	// �� ������
	float SplitChance = 0.5f;
	void TryCreatePartition();
	bool CanSplit();
	void SeparateHorizontal();
	void SeparateVertical();

	// �ǹ� �����
	void MakeOuterWall();
	void MakeInnerWall();

	// TODO: ���� ��ġ�ϱ� (�ٴ�, ��, õ��)
	// TODO: ����� ���� õ�� �߰��ϱ�
};
