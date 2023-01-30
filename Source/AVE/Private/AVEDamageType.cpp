// Fill out your copyright notice in the Description page of Project Settings.


#include "AVEDamageType.h"

UAVEDamageType::UAVEDamageType() {

}

float UAVEDamageType::CalculateDamage(float Damage, float Defense) {
	return Damage * (1 - Defense/(Defense + 1000));
}