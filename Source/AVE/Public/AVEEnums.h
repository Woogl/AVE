// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDamageType : uint8 
{
	EDT_Standard	UMETA(DisplayName = "Standard"),
	EDT_KnockBack	UMETA(DisplayName = "Knock Back"),
	EDT_KnockDown	UMETA(DisplayName = "Knock Down"),
	EDT_KnockUp		UMETA(DisplayName = "Knock Up"),
	EDT_NoReaction	UMETA(DisplayName = "No Reaction"),
};

UENUM(BlueprintType)
enum class EAttackType : uint8 
{
	Combo1,
	Combo2,
	Combo3,
	Combo4,
	RushAttack,
	SpecialAttack,
};

UENUM(BlueprintType)
enum class ECameraPosition : uint8 
{
	ECP_Default			UMETA(DisplayName = "Default View"),
	ECP_LeftSideView	UMETA(DisplayName = "Left Side View"),
	ECP_RightSideView	UMETA(DisplayName = "Right Side View"),
};

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