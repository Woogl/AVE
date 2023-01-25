// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorNode.h"

int32 FloorNode::FloorNodeCount = 0;

FloorNode::FloorNode()
{
	// 1類 蹺陛
	++FloorNodeCount;
}

FloorNode::FloorNode(const FCornerCoordinates& Coordinates)
{
	CornerCoordinates.UpperLeftX = Coordinates.UpperLeftX;
	CornerCoordinates.UpperLeftY = Coordinates.UpperLeftY;
	CornerCoordinates.LowerRightX = Coordinates.LowerRightX;
	CornerCoordinates.LowerRightY = Coordinates.LowerRightY;

	// 1類 蹺陛
	++FloorNodeCount;
}

FloorNode::~FloorNode()
{
	// 1類 馬模
	--FloorNodeCount;
}

FCornerCoordinates FloorNode::GetCornerCoordinates() const
{
	return CornerCoordinates;
}

void FloorNode::SetCornerCoordinates(FCornerCoordinates NewCoordinates)
{
	CornerCoordinates = NewCoordinates;
}

int32 FloorNode::GetFloorNodeCount()
{
	return FloorNodeCount;
}
