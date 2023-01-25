// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralFloor.h"
#include "FloorNode.h"

ProceduralFloor::ProceduralFloor()
{
}

ProceduralFloor::~ProceduralFloor()
{
}

TArray<TSharedPtr<FloorNode>> ProceduralFloor::GetPartitionedFloor()
{
	return PartitionedFloor;
}

void ProceduralFloor::MakePartition()
{
	// ��ǥ �����ϰ� ���ÿ� Ǫ��
	FCornerCoordinates CornerCoordinatesA = {0, 0, FloorGridSizeX, FloorGirdSizeY};
	FloorNodeStack.Push(TSharedPtr<FloorNode>(new FloorNode(CornerCoordinatesA)));

	while (FloorNodeStack.Num() > 0)
	{
		// �� ���ÿ��� ��Ƽ�� ���������� �� ����
		TSharedPtr<FloorNode> A = FloorNodeStack.Pop();	

		bool bNodeWasSplit = SplitAttempt(A);
		if (bNodeWasSplit == false)
		{
			PartitionedFloor.Push(A);
		}
	}
}

int32 ProceduralFloor::CoinFlip()
{
	// 0 �Ǵ� 1 ��ȯ
	return FMath::RandRange(0, 1);
}

bool ProceduralFloor::ShouldSplitNode(TSharedPtr<FloorNode> InNode, ESplitOrientation Orientation)
{
	FCornerCoordinates cornerCoordinates = InNode->GetCornerCoordinates();

	// ���η� �ڸ���
	if (Orientation == ESplitOrientation::ESO_Horizontal)
	{
		int32 floorLength = cornerCoordinates.LowerRightY - cornerCoordinates.UpperLeftY;
		float percentChanceOfSplit = (float)floorLength / FloorGirdSizeY;
		percentChanceOfSplit *= SplitChance;
		float diceRoll = FMath::FRandRange(0.f, 1.f);

		if (diceRoll > percentChanceOfSplit)
		{
			// �ڸ��� ����
			return false;
		}

		if (floorLength >= RoomMinY * 2)
		{
			// �ڸ��� ����
			return true;
		}
	}
	// ���η� �ڸ���
	else if (Orientation == ESplitOrientation::ESO_Vertical)
	{
		int32 floorWidth = cornerCoordinates.LowerRightX = cornerCoordinates.UpperLeftX;
		float percentChanceOfSplit = (float)floorWidth / FloorGridSizeX;
		percentChanceOfSplit *= SplitChance;
		float diceRoll = FMath::FRandRange(0.f, 1.f);

		if (diceRoll > percentChanceOfSplit)
		{
			// �ڸ��� ����
			return false;
		}

		if (floorWidth >= RoomMinX * 2)
		{
			// �ڸ��� ����
			return true;
		}
	}

	return false;
}

bool ProceduralFloor::SplitAttempt(TSharedPtr<FloorNode> InNode)
{
	int32 horizontalOrVertical = CoinFlip();
	if (horizontalOrVertical == 0)
	{
		if (ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			// ���η� �ڸ���
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			SplitHorizontal(InNode, B, C);
			return true;
		}
		else if (ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			// ���η� �ڸ���
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			SplitVertical(InNode, B, C);
			return true;
		}
	}
	else if (horizontalOrVertical == 1)
	{
		if (ShouldSplitNode(InNode, ESplitOrientation::ESO_Vertical))
		{
			// ���η� �ڸ���
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			SplitVertical(InNode, B, C);
			return true;
		}
		else if (ShouldSplitNode(InNode, ESplitOrientation::ESO_Horizontal))
		{
			// ���η� �ڸ���
			TSharedPtr<FloorNode> B(new FloorNode());
			TSharedPtr<FloorNode> C(new FloorNode());
			SplitHorizontal(InNode, B, C);
			return true;
		}
	}
	return false;
}

void ProceduralFloor::SplitHorizontal(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	// �ڸ��� �����ϴ� ������ ����
	int32 splitPointY = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftY + RoomMinY, InA->GetCornerCoordinates().LowerRightY + RoomMinY);

	// ���� ���� B�� ��ǥ
	FCornerCoordinates cornerCoordinatesB;
	cornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	cornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	cornerCoordinatesB.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	cornerCoordinatesB.LowerRightY = splitPointY;

	// B�� �߰�
	InB->SetCornerCoordinates(cornerCoordinatesB);
	FloorNodeStack.Push(InB);

	// ���� ���� C�� ��ǥ
	FCornerCoordinates cornerCoordinatesC;
	cornerCoordinatesC.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	cornerCoordinatesC.UpperLeftY = splitPointY;
	cornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	cornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	// C�� �߰�
	InC->SetCornerCoordinates(cornerCoordinatesC);
	FloorNodeStack.Push(InC);
}

void ProceduralFloor::SplitVertical(TSharedPtr<FloorNode> InA, TSharedPtr<FloorNode> InB, TSharedPtr<FloorNode> InC)
{
	// �ڸ��� �����ϴ� ������ ����
	int32 splitPointX = FMath::RandRange(InA->GetCornerCoordinates().UpperLeftX + RoomMinX, InA->GetCornerCoordinates().LowerRightX + RoomMinX);

	// ���� ���� B�� ��ǥ
	FCornerCoordinates cornerCoordinatesB;
	cornerCoordinatesB.UpperLeftX = InA->GetCornerCoordinates().UpperLeftX;
	cornerCoordinatesB.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	cornerCoordinatesB.LowerRightX = splitPointX;
	cornerCoordinatesB.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	// B�� �߰�
	InB->SetCornerCoordinates(cornerCoordinatesB);
	FloorNodeStack.Push(InB);

	// ���� ���� C�� ��ǥ
	FCornerCoordinates cornerCoordinatesC;
	cornerCoordinatesC.UpperLeftX = splitPointX;
	cornerCoordinatesC.UpperLeftY = InA->GetCornerCoordinates().UpperLeftY;
	cornerCoordinatesC.LowerRightX = InA->GetCornerCoordinates().LowerRightX;
	cornerCoordinatesC.LowerRightY = InA->GetCornerCoordinates().LowerRightY;

	// C�� �߰�
	InC->SetCornerCoordinates(cornerCoordinatesC);
	FloorNodeStack.Push(InC);
}

void ProceduralFloor::DrawFloorNodes(UWorld* World)
{
	for (int32 i = 0; i < PartitionedFloor.Num(); i++)
	{
		FCornerCoordinates Coordinates = PartitionedFloor[i]->GetCornerCoordinates();
		//DrawFloorNode(World, coordinates);

		const FVector upperLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.UpperLeftY * UnitLength, i * 10.f);
		const FVector upperRight(Coordinates.LowerRightX * UnitLength, Coordinates.UpperLeftY * UnitLength, i * 10.f);
		const FVector lowerLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.LowerRightY * UnitLength, i * 10.f);
		const FVector lowerRight(Coordinates.LowerRightX * UnitLength, Coordinates.LowerRightY * UnitLength, i * 10.f);

		DrawDebugLine(World, upperLeft, upperRight, FColor::Blue, true, -1.f, 0, 1.5f);
		DrawDebugLine(World, upperLeft, lowerLeft, FColor::Blue, true, -1.f, 0, 1.5f);
		DrawDebugLine(World, lowerLeft, lowerRight, FColor::Blue, true, -1.f, 0, 1.5f);
		DrawDebugLine(World, upperRight, lowerRight, FColor::Blue, true, -1.f, 0, 1.5f);
	}
}

void ProceduralFloor::DrawFloorNode(UWorld* World, FCornerCoordinates Coordinates)
{
	const FVector upperLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector upperRight(Coordinates.LowerRightX * UnitLength, Coordinates.UpperLeftY * UnitLength, 0.f);
	const FVector lowerLeft(Coordinates.UpperLeftX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);
	const FVector lowerRight(Coordinates.LowerRightX * UnitLength, Coordinates.LowerRightY * UnitLength, 0.f);

	DrawDebugLine(World, upperLeft, upperRight, FColor::Blue, true, -1.f, 0, 1.5f);
	DrawDebugLine(World, upperLeft, lowerLeft, FColor::Blue, true, -1.f, 0, 1.5f);
	DrawDebugLine(World, lowerLeft, lowerRight, FColor::Blue, true, -1.f, 0, 1.5f);
	DrawDebugLine(World, upperRight, lowerRight, FColor::Blue, true, -1.f, 0, 1.5f);
}
