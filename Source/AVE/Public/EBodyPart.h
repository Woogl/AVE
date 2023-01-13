// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
* ��ü���� � ������ �ڸ���
*/

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	EBP_Head		UMETA(DisplayName = "Head"),
	EBP_LeftArm		UMETA(DisplayName = "Left Arm"),
	EBP_RightArm	UMETA(DisplayName = "Right Arm"),
	EBP_Waist		UMETA(DisplayName = "Waist"),
	EBP_LeftLeg		UMETA(DisplayName = "Left Leg"),
	EBP_RightLeg	UMETA(DisplayName = "Right Leg"),
};