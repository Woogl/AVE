// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSMComponent.h"

#include "Boss.h"
#include "BossAnimInstance.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

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
	asBoss = Cast<ABoss>(GetOwner());

	asBossAnim = Cast<UBossAnimInstance>(asBoss->GetMesh()->GetAnimInstance());
}


// Called every frame
void UBossFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
	//int seqPlusValue = seqValue + 1;
	//int clampSeqValue = UKismetMathLibrary::Clamp(seqPlusValue, 1, 2);
	bDoOnce = true;
	if (bDoOnce == true)
	{
		IdleFSM();
		bDoOnce = false;
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, TEXT("TrueSuccecss"));
	}
	else
	{
		asBoss->SetZeroSpeed();
		asBoss->SetFocusPlayerTick();
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, TEXT("FalseSuccecss"));
	}
}

void UBossFSMComponent::TickWalk()
{
	SetWalkRandomInt(); // 한번만
	
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 125.f; // 계속
	
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

void UBossFSMComponent::IdleFSM()
{
	float idleRandomDelay = UKismetMathLibrary::RandomFloatInRange(0.5f, 1.5f);
	GetWorld()->GetTimerManager().SetTimer(idleTimerHandle, this, &UBossFSMComponent::SetWalkRandomInt, idleRandomDelay);
	// bossStates = EBossState::Walk;
	GetWorld()->GetTimerManager().ClearTimer(idleTimerHandle);
	// GetOwner()->GetWorldTimerManager().SetTimer(~~~)
}

void UBossFSMComponent::WalkFSM()
{
	float walkRandomDelay = UKismetMathLibrary::RandomFloatInRange(2.f, 4.f);
	GetWorld()->GetTimerManager().SetTimer(idleTimerHandle, this, &UBossFSMComponent::SetWalkRandomInt, walkRandomDelay);
	bossStates = EBossState::Walk;
}

void UBossFSMComponent::SetWalkRandomInt()
{
	walkRandomInt = UKismetMathLibrary::RandomIntegerInRange(1, 2);
	GetWorld()->GetTimerManager().SetTimer(walkRandomIntTimerHandle, this, &UBossFSMComponent::SelectRandomInt, 5.f, true);
}

void UBossFSMComponent::SelectRandomInt()
{
	walkRandomInt = UKismetMathLibrary::RandomIntegerInRange(1, 2);
}

