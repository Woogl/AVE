// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
* ī�޶� �̵��� �������� ��ġ
*/

UENUM(BlueprintType)
enum class ECameraPosition : uint8
{
	ECP_Default			UMETA(DisplayName = "Default View"),
	ECP_LeftSideView	UMETA(DisplayName = "Left Side View"),
	ECP_RightSideView	UMETA(DisplayName = "Right Side View"),
};
