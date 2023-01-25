// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

struct FCornerCoordinates
{
	int32 UpperLeftX;
	int32 UpperLeftY;
	int32 LowerRightX;
	int32 LowerRightY;
};

class AVE_API FloorNode
{
public:
	FloorNode();
	FloorNode(const FCornerCoordinates& Coordinates);
	~FloorNode();

	FCornerCoordinates CornerCoordinates;
	FCornerCoordinates GetCornerCoordinates() const;
	void SetCornerCoordinates(FCornerCoordinates NewCoordinates);

	static int32 FloorNodeCount;
	static int32 GetFloorNodeCount();
};