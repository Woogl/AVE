// Fill out your copyright notice in the Description page of Project Settings.


#include "HotelFloor.h"
#include <Kismet/KismetSystemLibrary.h>

AHotelFloor::AHotelFloor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// �ٴ�
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(RootComponent);

	// ��
	WallF = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallF"));
	WallF->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	WallF->SetupAttachment(Floor);

	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallB"));
	WallB->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
	WallB->SetupAttachment(Floor);

	WallL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallL"));
	WallL->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	WallL->SetupAttachment(Floor);

	WallR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallR"));
	WallR->SetupAttachment(Floor);

	// ���
	PillarFR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarFR"));
	PillarFR->SetRelativeRotation(FRotator(0.f, 135, 0.f));
	PillarFR->SetupAttachment(Floor);

	PillarFL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarFL"));
	PillarFL->SetRelativeRotation(FRotator(0.f, 225.f, 0.f));
	PillarFL->SetupAttachment(Floor);

	PillarBL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarBL"));
	PillarBL->SetRelativeRotation(FRotator(0.f, -45.f, 0.f));
	PillarBL->SetupAttachment(Floor);

	PillarBR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarBR"));
	PillarBR->SetRelativeRotation(FRotator(0.f, 45.f, 0.f));
	PillarBR->SetupAttachment(Floor);

	// ��
	DoorF = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorF"));
	DoorF->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	DoorF->SetupAttachment(RootComponent);

	DoorB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorB"));
	DoorB->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	DoorB->SetupAttachment(RootComponent);
}

void AHotelFloor::BeginPlay()
{
	Super::BeginPlay();

	//MakeExteriors();
	//CreateDebugGrid();
}

void AHotelFloor::SetupDimension(FVector InDimension)
{
	Dimension = InDimension;
}

void AHotelFloor::CreateDebugGrid()
{
	FVector bottomLeft = GetActorLocation();
	FVector topRight = bottomLeft + FVector(Dimension.X, Dimension.Y, 0.f);
	float debugHeight = GetActorLocation().Z + 5.f;

	// X�� �׸���
	for (int i = 0; i < GridNumX + 1; i++)
	{
		FVector start = bottomLeft + FVector(i * GridUnit, 0.f, debugHeight);
		FVector end = start + FVector(0.f, Dimension.Y, 0.f);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	}
	// Y�� �׸���
	for (int i = 0; i < GridNumX + 1; i++)
	{
		FVector start = bottomLeft + FVector(0.f, i * GridUnit, debugHeight);
		FVector end = start + FVector(Dimension.X, 0.f, 0.f);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, true);
	}
}

void AHotelFloor::TryCreatePartition()
{

}

bool AHotelFloor::CanSeparate()
{
	return false;
}

void AHotelFloor::SeparateHorizontal()
{

}

void AHotelFloor::SeparateVertical()
{

}

void AHotelFloor::MakeExteriors()
{
	// �� ũ�Ⱑ ��� ���ٰ� ������
	UKismetSystemLibrary::GetComponentBounds(Floor, FloorOrigin, FloorExtent, fNA);
	UKismetSystemLibrary::GetComponentBounds(WallF, vNA, WallExtent, fNA);
	UKismetSystemLibrary::GetComponentBounds(WallB, vNA, WallExtent, fNA);
	UKismetSystemLibrary::GetComponentBounds(WallL, vNA, WallExtent, fNA);
	UKismetSystemLibrary::GetComponentBounds(WallR, vNA, WallExtent, fNA);

	// �޽��� ��ǥ ũ�⿡ �°� ������ ����
	FVector newScale = FVector((Dimension.X / (FloorExtent.X * 2)), (Dimension.Y / (FloorExtent.Y * 2)), (Dimension.Z / (WallExtent.Z * 2)));
	Floor->SetRelativeScale3D(newScale);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("FloorExtent: %f %f %f"), FloorExtent.X, FloorExtent.Y, FloorExtent.Z));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("WallExtent: %f %f %f"), WallExtent.X, WallExtent.Y, WallExtent.Z));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("NewScale: %f %f %f"), newScale.X, newScale.Y, newScale.Z));

	// �� ��ġ ���� (����: WallExtent.Y = 10)
	WallF->SetRelativeLocation(FVector(-WallExtent.Y, 0.f, 0.f));
	WallB->SetRelativeLocation(FVector(FloorExtent.X * 2 + WallExtent.Y, FloorExtent.Y * 2, 0.f));
	WallL->SetRelativeLocation(FVector(FloorExtent.X * 2, -WallExtent.Y, 0.f));
	WallR->SetRelativeLocation(FVector(-WallExtent.Y, FloorExtent.Y * 2 + WallExtent.Y, 0.f));

	// ��� ��ġ ����
	PillarFL->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	PillarBL->SetRelativeLocation(FVector(FloorExtent.X * 2, 0.f, 0.f));
	PillarBR->SetRelativeLocation(FVector(FloorExtent.X * 2, FloorExtent.Y * 2, 0.f));
	PillarFR->SetRelativeLocation(FVector(0.f, FloorExtent.Y * 2, 0.f));

	// �� ��ġ ����
	DoorF->SetRelativeLocation(FVector(0.f, Dimension.Y * 0.5f, 0.f));
	DoorB->SetRelativeLocation(FVector(Dimension.X, Dimension.Y * 0.5f, 0.f));

	// �� �׸��� ���� ��� (�Ҽ��� ����)
	GridNumX = FMath::Floor(Dimension.X / GridUnit);
	GridNumY = FMath::Floor(Dimension.Y / GridUnit);
}

void AHotelFloor::MakeInteriors()
{

}

void AHotelFloor::DestroyInteriors()
{

}

