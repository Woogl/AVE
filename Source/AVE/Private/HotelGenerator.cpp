// Fill out your copyright notice in the Description page of Project Settings.


#include "HotelGenerator.h"
#include <Kismet/KismetSystemLibrary.h>

AHotelGenerator::AHotelGenerator()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 바닥
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetRelativeScale3D(HotelScale);
	Floor->SetupAttachment(RootComponent);

	// 벽
	WallF = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallF"));
	WallF->SetupAttachment(Floor);

	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallB"));
	WallB->SetRelativeLocationAndRotation(FVector(FloorMesh.X, FloorMeshY, 0.f), FRotator(0.f, 180.f, 0.f));
	WallB->SetupAttachment(Floor);

	WallL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallL"));
	WallL->SetRelativeLocationAndRotation(FVector(FloorMesh.X, 0.f, 0.f), FRotator(0.f, 90.f, 0.f));
	WallL->SetupAttachment(Floor);

	WallR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallR"));
	WallR->SetRelativeLocationAndRotation(FVector(0.f, FloorMeshY, 0.f), FRotator(0.f, -90.f, 0.f));
	WallR->SetupAttachment(Floor);

	// 기둥
	PillarFR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarFR"));
	PillarFR->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 0.f), FRotator(0.f, 225.f, 0.f));
	PillarFR->SetupAttachment(Floor);

	PillarFL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarFL"));
	PillarFL->SetRelativeLocationAndRotation(FVector(FloorMesh.X, 0.f, 0.f), FRotator(0.f, -45.f, 0.f));
	PillarFL->SetupAttachment(Floor);

	PillarBL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarBL"));
	PillarBL->SetRelativeLocationAndRotation(FVector(FloorMesh.X, FloorMeshY, 0.f), FRotator(0.f, 45.f, 0.f));
	PillarBL->SetupAttachment(Floor);

	PillarBR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarBR"));
	PillarBR->SetRelativeLocationAndRotation(FVector(0.f, FloorMeshY, 0.f), FRotator(0.f, 135.f, 0.f));
	PillarBR->SetupAttachment(Floor);

	// 문
	DoorF = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorF"));
	DoorF->SetRelativeLocationAndRotation(FVector(FloorSizeX * 0.5f, 0.f, 0.f), FRotator(0.f, 90.f, 0.f));
	DoorF->SetupAttachment(Root);

	DoorB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorB"));
	DoorB->SetRelativeLocationAndRotation(FVector(FloorSizeX * 0.5f, FloorSizeY, 0.f), FRotator(0.f, -90.f, 0.f));
	DoorB->SetupAttachment(Root);
}

void AHotelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	FVector FloorOrigin;
	FVector FloorBoxExtent;
	float notUsing;
	UKismetSystemLibrary::GetComponentBounds(Floor, FloorOrigin, FloorBoxExtent, notUsing);

	MakeOuterWall();
	CreateDebugGrid();
}

void AHotelGenerator::SetHotelSize(float InSizeX, float InSizeY, int InTotalFloors)
{
	// 호텔 너비 설정
	FloorSizeX = InSizeX;
	FloorSizeY = InSizeY;

	// 호텔 전체 층수 설정
	NumOfFloors = InTotalFloors;
}

void AHotelGenerator::CreateDebugGrid()
{
	FVector topLeft = GetActorLocation();
	FVector bottomRight = topLeft + FVector(FloorSizeX, FloorSizeY, 0.f);
	float currentHeight = GetActorLocation().Z + 5.f;
	
	// X축 그리드
	for (int32 i = 0; i < GridNumX + 1; i++)
	{
		FVector start = topLeft + FVector(i * UnitSize, 0.f, currentHeight);
		FVector end = start + FVector(0.f, FloorSizeY, currentHeight);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	}
	
	// Y축 그리드 
	for (int32 i = 0; i < GridNumY + 1; i++)
	{
		FVector start = topLeft + FVector(0.f, i * UnitSize, currentHeight);
		FVector end = start + FVector(FloorSizeX, 0.f, currentHeight);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	}
	
}

void AHotelGenerator::TryCreatePartition()
{

}

bool AHotelGenerator::CanSplit()
{
	return false;
}

void AHotelGenerator::SeparateHorizontal()
{

}

void AHotelGenerator::SeparateVertical()
{

}

void AHotelGenerator::MakeOuterWall()
{
	// 사이즈 조정
	FloorScaleX = FloorSizeX / FloorMesh.X;
	FloorScaleY = FloorSizeY / FloorMeshY;
	WallScaleZ = FloorHeightZ / WallMesh.Z;
	HotelScale = FVector(FloorScaleX, FloorScaleY, WallScaleZ);

	// 바닥 조정
	Floor->SetRelativeScale3D(HotelScale);

	// 벽 조정
	WallB->SetRelativeLocation(FVector(FloorMesh.X, FloorMeshY, 0.f));
	WallL->SetRelativeLocation(FVector(FloorMesh.X, 0.f, 0.f));
	WallR->SetRelativeLocation(FVector(0.f, FloorMeshY, 0.f));

	// 기둥 조정
	PillarFR->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PillarFL->SetRelativeLocation(FVector(FloorMesh.X, 0.f, 0.f));
	PillarBL->SetRelativeLocation(FVector(FloorMesh.X, FloorMeshY, 0.f));
	PillarBR->SetRelativeLocation(FVector(0.f, FloorMeshY, 0.f));

	// 문 조정
	DoorF->SetRelativeLocation(FVector(FloorSizeX * 0.5f, 0.f, 0.f));
	DoorB->SetRelativeLocation(FVector(FloorSizeX * 0.5f, FloorSizeY, 0.f));

	// 총 그리드 갯수 조정 (소수점 버림)
	GridNumX = FMath::Floor(FloorSizeX / UnitSize);
	GridNumY = FMath::Floor(FloorSizeY / UnitSize);
}

void AHotelGenerator::MakeInnerWall()
{

}

