// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSMComponent.h"

#include "AIController.h"
#include "Boss.h"
#include "BossAnimInstance.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
	auto playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	asPlayer = Cast<APlayerCharacter>(playerPawn);
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
	seqValue += 1;
	if (seqValue == 1)
	{
		IdleDelay();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("IdleTick"));
}

void UBossFSMComponent::TickWalk()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		SetWalkRandomInt();
		WalkDelay();
	}
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 125.f;
	WalkToLocation(1000);
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickMove()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		randomPercent = UKismetMathLibrary::RandomFloatInRange(0, 1);
	}
	SetMoveSpeed();
	asBoss->SetFocusPlayerTick();
	MoveToLocation();
	MoveToFSM();
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

void UBossFSMComponent::MoveToFSM()
{
	
	if (randomPercent <= dashATKPercent && asBoss->DistanceBossToPlayer() <= 700 && asBoss->DistanceBossToPlayer() > 600)
	{
		bossStates = EBossState::DashATK;
		seqValue = 0;
	}
	else if (asBoss->DistanceBossToPlayer() <= 250)
	{
		if (FMath::Abs(asBossAnim->yaw) > 120)
		{
			bossStates = EBossState::BehindATK;
			seqValue = 0;
		}
		else
		{
			bossStates = EBossState::NormalATK;
			seqValue = 0;
		}
	}
	else
	{
		bossStates = EBossState::Move;
		seqValue = 0;
	}
}

void UBossFSMComponent::SetMoveSpeed()
{
	if (asBoss->DistanceBossToPlayer() <= 400)
	{
		float decreaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() - 15;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 0, 700);
	}
	else
	{
		float increaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() + 15;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(increaseMaxSpeed, 0, 700);
	}
}
