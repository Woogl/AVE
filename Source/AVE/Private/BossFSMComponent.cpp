// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSMComponent.h"

// Sets default values for this component's properties
UBossFSMComponent::UBossFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBossFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	bossStates = EBossState::Idle;
}


// Called every frame
void UBossFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	switch (bossStates)
	{
	case EBossState::Idle: TickIdle(); break;
	case EBossState::Walk: TickWalk(); break;
	case EBossState::Move: TickMove(); break;
	case EBossState::DashATK: TickDashAtk(); break;
	case EBossState::NormalATK: TickNormalAtk(); break;
	case EBossState::ATK01: TickATK01(); break;
	case EBossState::ATK02: TickATK02(); break;
	case EBossState::ATK03: TickATK03(); break;
	case EBossState::JumpATK: TickJumpATK(); break;
	case EBossState::Backstep: TickBackstep(); break;
	case EBossState::BehindATK: TickBehindATK(); break;
	}
}

void UBossFSMComponent::TickIdle()
{
}

void UBossFSMComponent::TickWalk()
{
}

void UBossFSMComponent::TickMove()
{
}

void UBossFSMComponent::TickDashAtk()
{
}

void UBossFSMComponent::TickNormalAtk()
{
}

void UBossFSMComponent::TickATK01()
{
}

void UBossFSMComponent::TickATK02()
{
}

void UBossFSMComponent::TickATK03()
{
}

void UBossFSMComponent::TickJumpATK()
{
}

void UBossFSMComponent::TickBackstep()
{
}

void UBossFSMComponent::TickBehindATK()
{
}
