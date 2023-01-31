// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "AVEDamageType.generated.h"

/**
 * 
 */
UCLASS()
class AVE_API UAVEDamageType : public UDamageType
{
	GENERATED_BODY()
public:
	UAVEDamageType();
	static float CalculateDamage(float Damage, float Defense);
};
