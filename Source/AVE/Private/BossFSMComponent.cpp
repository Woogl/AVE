// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSMComponent.h"

#include "AIController.h"
#include "Boss.h"
#include "BossAnimInstance.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
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
	case EBossState::DashATK: TickDashATK(); break;
	case EBossState::NormalATK: TickNormalATK(); break;
	case EBossState::ATK01: TickATK01(); break;
	case EBossState::ATK02: TickATK02(); break;
	case EBossState::JumpATK: TickJumpATK(); break;
	case EBossState::StanceATK: TickStanceATK(); break;
	case EBossState::Backstep: TickBackstep(); break;
	case EBossState::BladeAura: TickBladeAura(); break;
	case EBossState::BehindATK: TickBehindATK(); break;
	}
	BossStateDebug();
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
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("IdleTick"));
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
		RandomFloat(0, 1);
	}
	SetMoveSpeed();
	asBoss->SetFocusPlayerTick();
	MoveToLocation();
	MoveToFSM();
}

void UBossFSMComponent::TickDashATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		RandomInt(1, 2);
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			if (randomIntValue == 1)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animDashATK01, EBossState::Move);
			}
			if (randomIntValue == 2)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animDashATK02, EBossState::Move);
			}
		}
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickNormalATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		RandomInt(1, 2);
		if (asBossAnim->IsAnyMontagePlaying() == false && bIsGuarding == false)
		{
			if (attackCount == 4)
			{
				bossStates = EBossState::JumpATK;
				seqValue = 0;
				attackCount = 0;
			}
			else if (randomIntValue == 1)
			{
				asBoss->OnMyPlayMontageNO(asBoss->animNormalATK01, EBossState::Move);
				attackCount++;
			}
			else if (randomIntValue == 2)
			{
				asBoss->OnMyPlayMontageNO(asBoss->animNormalATK02, EBossState::Move);
				attackCount++;
			}
		}
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickATK01()
{
}

void UBossFSMComponent::TickATK02()
{
}

void UBossFSMComponent::TickJumpATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->OnMyPlayMontageSA(asBoss->animJumpATK, EBossState::StanceATK);
		}
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickStanceATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		RandomInt(1, 2);
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			if (randomIntValue == 1)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animStanceToStab, EBossState::Move);
			}
			if (randomIntValue == 2)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animStanceToLowATK, EBossState::Walk);
			}
		}
	}
}

void UBossFSMComponent::TickBackstep()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		RandomInt(1, 2);
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			if (randomIntValue == 1)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animBackstep, EBossState::Walk);
			}
			if (randomIntValue == 2)
			{
				asBoss->OnMyPlayMontageSA(asBoss->animBackstep, EBossState::BladeAura);
			}
		}
		
	}
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickBladeAura()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->OnMyPlayMontageSA(asBoss->animBladeAura, EBossState::Walk);
			
		}
	}
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickBehindATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->OnMyPlayMontageSA(asBoss->animBehindATK, EBossState::Move);
		}
	}
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::MoveToFSM()
{
	if (asBossAnim->IsAnyMontagePlaying() == false)
	{
		if (randomFloatValue <= dashATKPercent && asBoss->DistanceBossToPlayer() <= 700 && asBoss->
			DistanceBossToPlayer() > 600)
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
		else ReturnToMove();
	}
}

void UBossFSMComponent::ReturnToMove()
{
	bossStates = EBossState::Move;
	seqValue = 0;
}

void UBossFSMComponent::ReturnToWalk()
{
	bossStates = EBossState::Walk;
	seqValue = 0;
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

void UBossFSMComponent::SetGuardMoveSpeed()
{
	if (asBoss->DistanceBossToPlayer() <= 400)
	{
		float decreaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() - 20;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 0, 700);
	}
	else
		asBoss->GetCharacterMovement()->MaxWalkSpeed = 125.f;
}

void UBossFSMComponent::GuardOrBackstep()
{
	if (asBoss->distanceValue <= 400)
	{
		RandomFloat(0, 1);
		if (randomFloatValue <= backstepPercent)
		{
			bossStates = EBossState::Backstep;
			seqValue = 0;
		}
		else GuardSelect();
	}
}

void UBossFSMComponent::RandomInt(int min, int max)
{
	randomIntValue = UKismetMathLibrary::RandomIntegerInRange(min, max);
}

void UBossFSMComponent::RandomFloat(float min, float max)
{
	randomFloatValue = UKismetMathLibrary::RandomFloatInRange(min, max);
}


