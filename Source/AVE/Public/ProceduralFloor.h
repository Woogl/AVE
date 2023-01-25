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

	// ���� ���� ������ ��� Stack
	TArray<TSharedPtr<class FloorNode>> FloorNodeStack;
	TArray<TSharedPtr<FloorNode>> PartitionedFloor;
	TArray<TSharedPtr<FloorNode>> GetPartitionedFloor();

	// �׸��� ����
	float UnitLength = 200.f;

	// ���� ���� Ȯ��
	float SplitChance = 0.5f;

	// ��ü ���� ũ��
	int32 FloorGridSizeX = 5;
	int32 FloorGirdSizeY = 5;

	// ���� �ּ� ũ��
	int32 RoomMinX = 1;
	int32 RoomMinY = 1;

	// �� ��ȹ�� ����
	void MakePartition();
	int32 CoinFlip();
	bool ShouldSplitNode(TSharedPtr<FloorNode> InNode, ESplitOrientation Orientation);
	bool SplitAttempt(TSharedPtr<FloorNode> InNode);
	// A �� ��ȹ�� ������ B�� C�� �����
	void SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);
	void SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC);

	void DrawFloorNodes(class UWorld* World);
	void DrawFloorNode(UWorld* World, FCornerCoordinates Coordinates);
};
