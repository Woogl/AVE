// Fill out your copyright notice in the Description page of Project Settings.


#include "HotelGenerator.h"
#include "HotelFloor.h"

AHotelGenerator::AHotelGenerator()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

void AHotelGenerator::BeginPlay()
{
	Super::BeginPlay();

}

void AHotelGenerator::SetupHotel(FVector EachFloorDimension, int InTotalFloor)
{
	// �� �� ũ�� ����
	FloorDimension = EachFloorDimension;
	// ��ü ���� ����
	TotalFloor = InTotalFloor;
}

void AHotelGenerator::BuildHotel()
{
	// ������ �ױ�
	for (int curFloor = 1; curFloor < TotalFloor + 1; curFloor++)
	{
		GenerateFloor(FloorDimension, curFloor);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GenerateFloor %d"), curFloor));
	}
}

void AHotelGenerator::GenerateFloor(FVector InFloorDimension, int CurFloor)
{
	// TODO: �κ�, ���� ����

	// BP Implementable Event
	SpawnFloor();
	
	// ������ �ױ� ���� ����
	FVector newLocation = GetActorLocation();
	newLocation.Z += FloorDimension.Z;
	SetActorLocation(newLocation);
}