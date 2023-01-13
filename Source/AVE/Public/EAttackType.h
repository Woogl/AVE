// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
* ���� Ÿ�Ը��� �ٸ� �ǰ� ���� 
*/

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	EAT_Standard		UMETA(DisplayName = "Standard"),
	EAT_KnockBack		UMETA(DisplayName = "Knock Back"),
	EAT_KnockDown			UMETA(DisplayName = "Knock Down"),
	EAT_KnockUp		UMETA(DisplayName = "Knock Up"),
	EAT_NoReaction		UMETA(DisplayName = "No Reaction"),
};