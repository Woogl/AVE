// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloorNode.h"

/**
 * 
 */
UENUM()
enum class ESplitOrientation : uint8
{
	ESO_Horizontal,
	ESO_Vertical
};

class AVE_API ProceduralFloor
{
public:
	ProceduralFloor();
	~ProceduralFloor();

	// 층과 방의 정보를 담는 Stack
	TArray<TSharedPtr<class FloorNode>> FloorNodeStack;
	TArray<TSharedPtr<FloorNode>> PartitionedFloor;
	TArray<TSharedPtr<FloorNode>> GetPartitionedFloor();

	// 그리드 단위
	float UnitLength = 200.f;

	// 방을 나눌 확률
	float SplitChance = 0.5f;

	// 전체 층의 크기
	int32 FloorGridSizeX = 5;
	int32 FloorGirdSizeY = 5;

	// 방의 최소 크기
	int32 RoomMinX = 1;
	int32 RoomMinY = 1;

	// 방 구획을 나눔
	void MakePartition();
	int32 CoinFlip();
	bool ShouldSplitNode(TSharedPtr<FloorNode> InNode, ESplitOrientation Orientation);
	bool SplitAttempt(TSharedPtr<FloorNode> InNode);
	// A 방 구획을 나누면 B와 C가 생긴다
	void SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);
	void SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);

	void DrawFloorNodes(class UWorld* World);
	void DrawFloorNode(UWorld* World, FCornerCoordinates Coordinates);
};
