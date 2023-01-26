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
	// °¢ Ãþ Å©±â ¼³Á¤
	FloorDimension = EachFloorDimension;
	// ÀüÃ¼ Ãþ¼ö ¼³Á¤
	TotalFloor = InTotalFloor;
}

void AHotelGenerator::BuildHotel()
{
	// ¿©·¯Ãþ ½×±â
	for (int curFloor = 1; curFloor < TotalFloor + 1; curFloor++)
	{
		GenerateFloor(FloorDimension, curFloor);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("GenerateFloor %d"), curFloor));
	}
}

void AHotelGenerator::GenerateFloor(FVector InFloorDimension, int CurFloor)
{
	// TODO: ·Îºñ, ¿Á»ó ½ºÆù

	// BP Implementable Event
	SpawnFloor();
	
	// ´ÙÀ½Ãþ ½×±â À§ÇØ À§·Î
	FVector newLocation = GetActorLocation();
	newLocation.Z += FloorDimension.Z;
	SetActorLocation(newLocation);
}