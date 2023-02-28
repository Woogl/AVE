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
	bIsSecondPhase = false;
	bHasExecuted = false;
	//asBoss->GetWorldTimerManager().SetTimer(secondPhaseHandle, this, &UBossFSMComponent::ReturnToSecondPhase, 0.1f, true);
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
	case EBossState::LightningATK: TickLightningATK(); break;
	case EBossState::SlashATK: TickSlashATK(); break;
	case EBossState::ComboATK: TickComboATK(); break;
	case EBossState::JumpATK: TickJumpATK(); break;
	case EBossState::StanceATK: TickStanceATK(); break;
	case EBossState::BackStep: TickBackStep(); break;
	case EBossState::BladeRangeATK: TickBladeRangeATK(); break;
	case EBossState::BehindATK: TickBehindATK(); break;
	case EBossState::GrabATK: TickGrabATK(); break;
	case EBossState::WarCry: TickWarCry(); break;
	case EBossState::FallDown: TickFallDown(); break;
	case EBossState::LaserRangeATK: TickLaserRangeATK(); break;
	}
	BossStateDebug();
	
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("bIsSecondPhase %d"), bIsSecondPhase));
	ReturnToSecondPhase();
}

void UBossFSMComponent::TickIdle()
{
	if (bHasExecuted == false)
	{
		IdleDelay();
		bHasExecuted = true;
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("IdleTick"));
}

void UBossFSMComponent::TickWalk()
{
	if (bHasExecuted == false)
	{
		SetWalkRandomInt();
		WalkDelay();
		bHasExecuted = true;
	}
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 125.f;
	WalkToLocation(1000);
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickMove()
{
	if (bHasExecuted == false)
	{
		RandomFloat();
		bHasExecuted = true;
	}
	SetMoveSpeed();
	asBoss->SetFocusPlayerTick();
	MoveToLocation();
	MoveToFSM();
}

void UBossFSMComponent::TickDashATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimDashATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
				asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickNormalATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			// if (asBoss->attackCount == 2)
			// {
			// 	bossStates = EBossState::JumpATK;
			// 	asBoss->attackCount += 1;
			// 	bHasExecuted = false;
			// }
			// else if (asBoss->attackCount == 4)
			// {
			// 	bossStates = EBossState::ComboATK;
			// 	asBoss->attackCount += 1;
			// 	bHasExecuted = false;
			// 	//asBoss->attackCount = 0;
			// }
			// else if (asBoss->attackCount == 7)
			// {
			// 	bossStates = EBossState::GrabATK;
			// 	asBoss->attackCount += 1;
			// 	bHasExecuted = false;
			// 	//asBoss->attackCount = 0;
			// }
			// else if (asBoss->attackCount >= 9)
			// {
			// 	bossStates = EBossState::BackStep;
			// 	asBoss->attackCount = 0;
			// 	bHasExecuted = false;
			// }
			// else
			// {
				asBoss->AnimNormalATK();
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove, asBoss->montageLength, false);
			// }
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerInplace();
}

void UBossFSMComponent::TickLightningATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimLightningATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToWalk,
							asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerPitchYaw();
}

void UBossFSMComponent::TickSlashATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimSlashATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
							asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerInplace();
}

void UBossFSMComponent::TickComboATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false || asBossAnim->bParry == true)
		{
			if (asBossAnim->bParry == true)
			{
				asBossAnim->bParry = false;
			}
			asBoss->AnimComboATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
							asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickJumpATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false || asBossAnim->bParry == true)
		{
			if (asBossAnim->bParry == true)
			{
				asBossAnim->bParry = false;
			}
			asBoss->AnimJumpATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToStanceATK,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickStanceATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimStanceATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickBackStep()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false || asBossAnim->bParry == true)
		{
			if (asBossAnim->bParry == true)
			{
				asBossAnim->bParry = false;
			}
			asBoss->AnimBackStep();
			if (bIsSecondPhase == false) // && randomFloatValue <= backStepPercent
			{
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToBladeRangeATK,
					asBoss->montageLength, false);
			}
			else if (bIsSecondPhase == true) // && randomFloatValue <= laserRangeATKPercent
			{
				if (randomFloatValue <= laserRangeATKPercent)
				{
					asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToLaserRangeATK,
					asBoss->montageLength, false);
				}
				else
				{
					asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToLightningATK,
					asBoss->montageLength, false);
				}
			}
			else
				asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToWalk,
					asBoss->montageLength, false);
		}
		else ReturnToWalk();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickBladeRangeATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimBladeRangeATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
				asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickBehindATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimBehindATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
	asBoss->SetFocusPlayerTick();
}

void UBossFSMComponent::TickGrabATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false || asBossAnim->bParry == true)
		{
			if (asBossAnim->bParry == true)
			{
				asBossAnim->bParry = false;
			}
			asBoss->AnimGrabATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->SetZeroSpeed();
}

void UBossFSMComponent::TickWarCry()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimWarCry();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToMove,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void UBossFSMComponent::TickFallDown()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		asBoss->AnimFallDown();
		asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToWarCry,
						asBoss->montageLength, false);
	}
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void UBossFSMComponent::TickLaserRangeATK()
{
	if (bHasExecuted == false)
	{
		bHasExecuted = true;
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			asBoss->AnimLaserRangeATK();
			asBoss->GetWorldTimerManager().SetTimer(delayHandle, this, &UBossFSMComponent::ReturnToWalk,
					asBoss->montageLength, false);
		}
		else ReturnToMove();
	}
	asBoss->GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void UBossFSMComponent::MoveToFSM()
{
	if (asBossAnim->IsAnyMontagePlaying() == true)return;

	if (asBoss->DistanceBossToPlayer() <= 250)
	{
		if (asBossAnim->yaw > 90 || asBossAnim->yaw < -90)
		{
			bossStates = EBossState::BehindATK;
			bHasExecuted = false;
		}
		else
		{
			ReturnToNormalATK();
		}
	}
	else if (randomFloatValue <= dashATKPercent && asBoss->DistanceBossToPlayer() <= 700 && asBoss->
			DistanceBossToPlayer() > 600)
	{
		if (bIsSecondPhase == true)
		{
			if (asBoss->currentElectricEnergy >= 0 && asBoss->currentElectricEnergy < 70)
			{
				bossStates = EBossState::LightningATK;
				bHasExecuted = false;
			}
			else
			{
				bossStates = EBossState::DashATK;
				bHasExecuted = false;
			}
		}
		else
		{
			bossStates = EBossState::DashATK;
			bHasExecuted = false;
		}
	}
	else if (randomFloatValue <= slashATKPercent && asBoss->DistanceBossToPlayer() <= 500 && asBoss->
			DistanceBossToPlayer() > 400)
	{
		bossStates = EBossState::SlashATK;
		bHasExecuted = false;
	}
	else bossStates = EBossState::Move; // 랜덤플롯을 실행시키지 않기 위해
}

void UBossFSMComponent::ReturnToMove()
{
	bossStates = EBossState::Move;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToWalk()
{
	bossStates = EBossState::Walk;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToNormalATK()
{
	bossStates = EBossState::NormalATK;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToBladeRangeATK()
{
	bossStates = EBossState::BladeRangeATK;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToStanceATK()
{
	bossStates = EBossState::StanceATK;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToSlashATK()
{
	bossStates = EBossState::SlashATK;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToSecondPhase()
{
	if (bIsSecondPhase == false)
	{
		if (asBoss->bossPosture <= 0)
		{
			bossStates = EBossState::FallDown;
			bHasExecuted = false;
			bIsSecondPhase = true;
			asBoss->GetWorldTimerManager().ClearTimer(secondPhaseHandle);
		}
	}
}

void UBossFSMComponent::ReturnToWarCry()
{
	bossStates = EBossState::WarCry;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToLaserRangeATK()
{
	bossStates = EBossState::LaserRangeATK;
	bHasExecuted = false;
}

void UBossFSMComponent::ReturnToLightningATK()
{
	bossStates = EBossState::LightningATK;
	bHasExecuted = false;
}

void UBossFSMComponent::SetMoveSpeed()
{
	float currentSpeed = asBoss->GetCharacterMovement()->MaxWalkSpeed;
	if (asBoss->DistanceBossToPlayer() <= 400)
	{
		float decreaseMaxSpeed = currentSpeed - 10.f;
		asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 125, 700);
		if (asBoss->DistanceBossToPlayer() <= 250)
		{
			decreaseMaxSpeed = currentSpeed - 15.f;
			asBoss->GetCharacterMovement()->MaxWalkSpeed = UKismetMathLibrary::FClamp(decreaseMaxSpeed, 0, 125);
		}
	}
	else
	{
		float increaseMaxSpeed = currentSpeed + 10.f;
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

void UBossFSMComponent::GuardOrBackStep()
{
	if (asBoss->DistanceBossToPlayer() <= 400)
	{
		RandomFloat();
		if (randomFloatValue <= backStepPercent)
		{
			bossStates = EBossState::BackStep;
			bHasExecuted = false;
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
