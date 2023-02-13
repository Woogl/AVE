// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFSMComponent.h"

#include "AIController.h"
#include "Boss.h"
#include "BossAnimInstance.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	case EBossState::SlashATK: TickSlashATK(); break;
	case EBossState::ComboATK: TickComboATK(); break;
	case EBossState::JumpATK: TickJumpATK(); break;
	case EBossState::StanceATK: TickStanceATK(); break;
	case EBossState::BackStep: TickBackStep(); break;
	case EBossState::BladeRangeATK: TickBladeRangeATK(); break;
	case EBossState::BehindATK: TickBehindATK(); break;
	case EBossState::GrabATK: TickGrabATK(); break;
	}
	BossStateDebug();
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("attackCount %d"), attackCount));
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("randomIntValue %d"), randomIntValue));
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
		RandomFloat();
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
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimDashATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
				1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickNormalATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false && bIsGuarding == false)
		{
			if (asBoss->attackCount == 2)
			{
				bossStates = EBossState::JumpATK;
				asBoss->attackCount += 1;
				seqValue = 0;
			}
			else if (asBoss->attackCount == 4)
			{
				bossStates = EBossState::ComboATK;
				asBoss->attackCount += 1;
				seqValue = 0;
				//asBoss->attackCount = 0;
			}
			else if (asBoss->attackCount == 7)
			{
				bossStates = EBossState::GrabATK;
				asBoss->attackCount += 1;
				seqValue = 0;
				//asBoss->attackCount = 0;
			}
			else if (attackCount >= 9)
			{
				bossStates = EBossState::BackStep;
				attackCount = 0;
				seqValue = 0;
			}
			else
			{
				asBoss->AnimNormalATK();
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					1.f, false, asBoss->montageLength);
			}
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickATK01()
{
}

void UBossFSMComponent::TickSlashATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimSlashATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
							1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickComboATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimComboATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
							1.f, false, asBoss->montageLength);
		}
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickJumpATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimJumpATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToStanceATK,
					1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickStanceATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimStanceATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickBackStep()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimBackStep();
			if (randomFloatValue <= backStepPercent)
			{
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToBladeRangeATK,
					1.f, false, asBoss->montageLength);
			}
			else
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickBladeRangeATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimBladeRangeATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, FTimerDelegate::CreateLambda([&]()
			{
				asBoss->weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
				ReturnToWalk();
				GetOwner()->GetWorldTimerManager().ClearTimer(delayHandle);
			}), 1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickBehindATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimBehindATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickGrabATK()
{
	seqValue += 1;
	if (seqValue == 1)
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimGrabATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					1.f, false, asBoss->montageLength);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
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
		else if (randomFloatValue <= slashATKPercent && asBoss->DistanceBossToPlayer() <= 400 && asBoss->
			DistanceBossToPlayer() > 350)
		{
			bossStates = EBossState::SlashATK;
			seqValue = 0;
		}
		else bossStates = EBossState::Move; // 랜덤 플롯을 계속 실행시키지 않기 위해
		
		if (asBoss->DistanceBossToPlayer() <= 250)
		{
			float yawABS = FMath::Abs(asBossAnim->yaw);
			if (yawABS > 100)
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
	}
}

void UBossFSMComponent::ReturnToMove()
{
	asBoss->bIsSuperArmor = false;
	bossStates = EBossState::Move;
	seqValue = 0;
}

void UBossFSMComponent::ReturnToWalk()
{
	asBoss->bIsSuperArmor = false;
	bossStates = EBossState::Walk;
	seqValue = 0;
}

void UBossFSMComponent::ReturnToBladeRangeATK()
{
	bossStates = EBossState::BladeRangeATK;
	seqValue = 0;
}

void UBossFSMComponent::ReturnToStanceATK()
{
	bossStates = EBossState::StanceATK;
	seqValue = 0;
}

void UBossFSMComponent::ReturnToSlashATK()
{
	bossStates = EBossState::SlashATK;
	seqValue = 0;
}

void UBossFSMComponent::SetMoveSpeed()
{
	if (asBoss->DistanceBossToPlayer() <= 600)
	{
		float decreaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() - 5.f;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 125, 700);
		//asBossAnim->activeChildIndex = 1;
		if (asBoss->DistanceBossToPlayer() <= 250)
		{
			decreaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() - 5.f;
			asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 0.f, 700.f);
		}
	}
	else
	{
		float increaseMaxSpeed = asBoss->GetCharacterMovement()->GetMaxSpeed() + 15;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(increaseMaxSpeed, 0, 700);
		//asBossAnim->activeChildIndex = 0;
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

void UBossFSMComponent::GuardOrBackStep()
{
	if (asBoss->DistanceBossToPlayer() <= 400)
	{
		RandomFloat();
		if (randomFloatValue <= backStepPercent)
		{
			bossStates = EBossState::BackStep;
			seqValue = 0;
		}
		//else GuardSelect();
	}
}

void UBossFSMComponent::RandomInt(int min, int max)
{
	randomIntValue = UKismetMathLibrary::RandomIntegerInRange(min, max);
}

void UBossFSMComponent::RandomFloat()
{
	randomFloatValue = UKismetMathLibrary::RandomFloatInRange(0, 1);
}
