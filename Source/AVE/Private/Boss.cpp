// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "BossAnimInstance.h"
#include "BossFSMComponent.h"
#include "CombatComponent.h"
#include "GanpaDamageType.h"
#include "MovieScene.h"
#include "ParryDamageType.h"
#include "PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnitConversion.h"

// Sets default values
ABoss::ABoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//캐릭터 메쉬 생성
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempbody(TEXT("SkeletalMesh'/Game/MarketplaceAssets/Robot2/Mesh/Robot2.Robot2'"));
    if (tempbody.Succeeded())
    {
	    GetMesh()->SetSkeletalMesh(tempbody.Object);
    	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -117, 0));
    }
	
	//스켈레탈 메쉬 무기 생성
	weaponMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("weaponMeshComp"));
	weaponMeshComp->SetupAttachment(GetMesh(), TEXT("BossWeapon"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempWeaponMesh(TEXT("SkeletalMesh'/Game/Team/KH/Katana_01/SK_Katana_01_A.SK_Katana_01_A'"));
    if (tempWeaponMesh.Succeeded())
    {
	    weaponMeshComp->SetSkeletalMesh(tempWeaponMesh.Object);
    	weaponMeshComp->SetRelativeScale3D(FVector(1.f, 1.2f, 1.2f));
    }

	//스태틱 메쉬 무기 생성
	weaponMeshSubComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("weaponMeshSubComp"));
	weaponMeshSubComp->SetupAttachment(weaponMeshComp, TEXT("BossWeapon"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempWeaponSubMesh(TEXT("StaticMesh'/Game/Team/KH/Katana_01/SM_Katana_01_B.SM_Katana_01_B'"));
    if (tempWeaponSubMesh.Succeeded())
    {
	    weaponMeshSubComp->SetStaticMesh(tempWeaponSubMesh.Object);
    	weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
    }

	//Combat Component 생성
	combatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("combatComp"));

	//BossFSM Component 생성 -> BP_BossFSM 에서 블루프린트와 조합해서 사용
	bossFSMComp = CreateDefaultSubobject<UBossFSMComponent>(TEXT("bossFSMComp"));
	
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	combatComp->SetupWeapon(weaponMeshSubComp);

	currentHP = maxHP;
	bossPosture = bossMaxPosture;
	
	asBossAnim = Cast<UBossAnimInstance>(GetMesh()->GetAnimInstance());
	playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	asPlayer = Cast<APlayerCharacter>(playerPawn);

	attackCount = 0;
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DistanceBossToPlayer();
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("currentHP %f"), currentHP));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("bossPosture %f"), bossPosture));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("attackCount %d"), attackCount));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("TakeDamage %d"), bTakeDamage));
	
	if (asBossAnim->IsAnyMontagePlaying() == false)
	{
		//GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::PostureRecovery, 0.3f, false);
		PostureRecovery();
	}
}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::SetZeroSpeed()
{
	// float decreaseWalkSpeed = GetCharacterMovement()->GetMaxSpeed() - 10.f;
	// float walkSpeedClamp = UKismetMathLibrary::FClamp(decreaseWalkSpeed, 0.f, 500.f);
	// GetCharacterMovement()->MaxWalkSpeed = walkSpeedClamp;
	float Deceleration = 100.f;
	float CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
	float SpeedDelta = 700.f - CurrentSpeed;
	float Decel = FMath::Min(SpeedDelta, Deceleration * GetWorld()->GetDeltaSeconds());
	CurrentSpeed -= Decel;
	FMath::Max(CurrentSpeed, 0.0f);
}

// void ABoss::BossPlayMontageSA(UAnimMontage* selectMontage)
// {
// 	bIsSuperArmor = true;
// 	GetMesh()->GetAnimInstance()->Montage_Play(selectMontage);
// 	montageLength = EMontagePlayReturnType::MontageLength(selectMontage);
// 	if (DistanceBossToPlayer() > 400)
// 	{
// 		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToWalk, montageLength, false);
// 	}
// 	else
// 		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
// }

void ABoss::AnimDashATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		bIsSuperArmor = true;
		//PlayAnimMontage(animDashATK01);
		montageLength = PlayAnimMontage(animDashATK01, 1) / (1 * animDashATK01->RateScale);
	}
	if (randomIntValue == 2)
	{
		bIsSuperArmor = true;
		//PlayAnimMontage(animDashATK02);
		montageLength = PlayAnimMontage(animDashATK02, 1) / (1 * animDashATK02->RateScale);
	}
}

void ABoss::AnimNormalATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		//PlayAnimMontage(animNormalATKR01);
		attackCount += 1;
		montageLength = PlayAnimMontage(animNormalATKR01, 1) / (1 * animNormalATKR01->RateScale);
	}
	if (randomIntValue == 2)
	{
		//PlayAnimMontage(animNormalATKL01);
		attackCount += 1;
		montageLength = PlayAnimMontage(animNormalATKL01, 1) / (1 * animNormalATKL01->RateScale);
	}
}

void ABoss::AnimJumpATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animJumpATK);
	montageLength = PlayAnimMontage(animJumpATK, 1) / (1 * animJumpATK->RateScale);
}

void ABoss::AnimStanceATK()
{
	bIsSuperArmor = true;
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		//PlayAnimMontage(animStanceToStab);
		montageLength = PlayAnimMontage(animStanceToStab, 1) / (1 * animStanceToStab->RateScale);
	}
	if (randomIntValue == 2)
	{
		//PlayAnimMontage(animStanceToLowATK);
		montageLength = PlayAnimMontage(animStanceToLowATK, 1) / (1 * animStanceToLowATK->RateScale);
		Tags.Add(TEXT("LowATK"));
		MyDebug("On LowATK");
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ClearTags, montageLength, false);
	}
}

void ABoss::AnimBehindATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animBehindATK);
	montageLength = PlayAnimMontage(animBehindATK, 1) / (1 * animBehindATK->RateScale);
}

void ABoss::AnimBackStep()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animBackStep);
	montageLength = PlayAnimMontage(animBackStep, 1) / (1 * animBackStep->RateScale);
}

void ABoss::AnimBladeRangeATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animBladeRangeATK);
	weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 5.f, 1.f));
	montageLength = PlayAnimMontage(animBladeRangeATK, 1) / (1 * animBladeRangeATK->RateScale);
}

void ABoss::AnimGrabATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animGrabATK);
	montageLength = PlayAnimMontage(animGrabATK, 1) / (1 * animGrabATK->RateScale);
	Tags.Add(TEXT("GrabATK"));
	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ClearTags, montageLength, false);
}

void ABoss::AnimSlashATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animSlashATK);
	montageLength = PlayAnimMontage(animSlashATK, 1) / (1 * animSlashATK->RateScale);
}

void ABoss::AnimComboATK()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animComboATK);
	montageLength = PlayAnimMontage(animComboATK, 1) / (1 * animComboATK->RateScale);
}

void ABoss::AnimWarCry()
{
	bIsSuperArmor = true;
	//PlayAnimMontage(animWarCry);
	montageLength = PlayAnimMontage(animWarCry, 1) / (1 * animWarCry->RateScale);
	Tags.Add(TEXT("UnableGuard"));
	MyDebug("On UnableGuard");
	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ClearTags, montageLength, false);
}

void ABoss::AnimInsal()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animInsal, 1) / (1 * animInsal->RateScale);
}

void ABoss::AnimLaserRangeATK()
{
	montageLength = PlayAnimMontage(animLaserRangeATK, 1) / (1 * animLaserRangeATK->RateScale);
}

// void ABoss::BossPlayMontage(UAnimMontage* selectMontage)
// {
// 	PlayAnimMontage(selectMontage);
// 	montageLength = PlayAnimMontage(selectMontage, 1) / (1 * selectMontage->RateScale);
// 	//montageLength = EMontagePlayReturnType::MontageLength(selectMontage);
// 	// if (DistanceBossToPlayer() > 400)
// 	// {
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToWalk, montageLength, false);
// 	// }
// 	// else if (bossFSMComp->bossStates == EBossState::Backstep)
// 	// {
// 	// 	if (bossFSMComp->randomIntValue)
// 	// 	{
// 	// 		
// 	// 	}
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToBladeRangeATK, montageLength, false);
// 	// }
// 	// else
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
// }
//
// void ABoss::ReturnToMove()
// {
// 	bIsSuperArmor = false;
// 	bossFSMComp->bossStates = EBossState::Move;
// 	bossFSMComp->bHasExecuted = false;
// }
//


void ABoss::RandomInt(int min, int max)
{
	randomIntValue = UKismetMathLibrary::RandomIntegerInRange(min, max);
}

void ABoss::RandomFloat()
{
	randomFloatValue = UKismetMathLibrary::RandomFloatInRange(0, 1);
}

void ABoss::AnimTurnInPlace()
{
	if (asBossAnim->IsAnyMontagePlaying() == false)
	{
		if (asBossAnim->yaw > 80)
		{
			if (asBossAnim->yaw > 135)
				PlayAnimMontage(bossTurnR180);
			else
				PlayAnimMontage(bossTurnR90);
		}
		else if (asBossAnim->yaw < -80)
		{
			if (asBossAnim->yaw < -135)
				PlayAnimMontage(bossTurnL180);
			else
				PlayAnimMontage(bossTurnL90);
		}
	}
}

void ABoss::SetFocusPlayerInplace()
{
	FRotator bossLookAtPlayer = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), playerPawn->GetActorLocation());
	FRotator setLerp = UKismetMathLibrary::RLerp(GetActorRotation(), bossLookAtPlayer, 0.1f, true);
	this->SetActorRotation(FRotator(0, setLerp.Yaw, 0));
}

void ABoss::SetFocusPlayerTick()
{
	if (asBossAnim->speed > 100)
	{
		SetFocusPlayerInplace();
	}
	else
	{
		AnimTurnInPlace();
	}
}

void ABoss::OnLineTraceHit()
{
	FVector lineStartLoc = GetMesh()->GetSocketLocation(FName("head"));
	FVector lineEndLoc = lineStartLoc + (GetActorForwardVector() * 300000.f);
	FHitResult outHit;
	AController* instigator = GetController();
	ETraceTypeQuery AttackTrace = TraceTypeQuery5;
	TArray<AActor*> IgnoreList;
	
	// bool bHit = GetWorld()->LineTraceSingleByChannel(outHit, lineStartLoc, lineEndLoc, ECollisionChannel::ECC_Visibility);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, lineStartLoc, lineEndLoc, AttackTrace,
		false, IgnoreList, EDrawDebugTrace::None, outHit, true);
	if (bHit)
	{
		EnemyTarget = outHit.GetActor();
		UGameplayStatics::ApplyPointDamage(EnemyTarget, 5.f, EnemyTarget->GetActorLocation(),
			outHit, instigator, this, UKnockBackDamageType::StaticClass());
	}
}

void ABoss::TakeDamageFalse()
{
	bTakeDamage = false;
}

void ABoss::ClearTags()
{
	Tags.Empty();
}

void ABoss::ReturnToMove()
{
	bIsSuperArmor = false;
	bossFSMComp->bossStates = EBossState::Move;
	bossFSMComp->bHasExecuted = false;
}

void ABoss::ReturnToWalk()
{
	bIsSuperArmor = false;
	bossFSMComp->bossStates = EBossState::Walk;
	bossFSMComp->bHasExecuted = false;
}

void ABoss::ReturnToBladeRangeATK()
{
	bossFSMComp->bossStates = EBossState::BladeRangeATK;
	bossFSMComp->bHasExecuted = false;
}

void ABoss::AnimReboundATK()
{
	if (attackCount == 2)
	{
		bossFSMComp->bossStates = EBossState::JumpATK;
		attackCount += 1;
		bossFSMComp->bHasExecuted = false;
	}
	else if (attackCount == 4)
	{
		bossFSMComp->bossStates = EBossState::ComboATK;
		attackCount += 1;
		bossFSMComp->bHasExecuted = false;
	}
	else if (attackCount == 7)
	{
		bossFSMComp->bossStates = EBossState::GrabATK;
		attackCount += 1;
		bossFSMComp->bHasExecuted = false;
	}
	else if (attackCount >= 9)
	{
		bossFSMComp->bossStates = EBossState::BackStep;
		attackCount = 0;
		bossFSMComp->bHasExecuted = false;
	}
	else if (bossPosture <= 0)
	{
		bossFSMComp->bossStates = EBossState::Insal;
		bossFSMComp->bHasExecuted = false;
		bossFSMComp->bIsSecondPhase = true;
	}
	else
	{
		if (randomIntValue == 1)
		{
			//PlayAnimMontage(animReboundATKL);
			montageLength = PlayAnimMontage(animReboundATKL, 1) / (1 * animReboundATKL->RateScale);
			attackCount += 1;
			GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
		}
		if (randomIntValue == 2)
		{
			//PlayAnimMontage(animReboundATKR);
			montageLength = PlayAnimMontage(animReboundATKR, 1) / (1 * animReboundATKR->RateScale);
			attackCount += 1;
			GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
		}
	}
}

// void ABoss::AnimParryATK()
// {
// 	if (randomIntValue == 1)
// 	{
// 		PlayAnimMontage(animParryRATK);
// 		montageLength = PlayAnimMontage(animParryRATK, 1) / (1 * animParryRATK->RateScale);
// 		attackCount += 1;
// 	}
// 	if (randomIntValue == 2)
// 	{
// 		PlayAnimMontage(animParryLATK);
// 		montageLength = PlayAnimMontage(animParryLATK, 1) / (1 * animParryLATK->RateScale);
// 		attackCount += 1;
// 	}
// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
// }

float ABoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float bossTakenDamage = DamageAmount / bossArmor;
	bTakeDamage = true;
	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::TakeDamageFalse, 1.f, false);
	
	if (DamageEvent.DamageTypeClass == UParryDamageType::StaticClass() && bIsSuperArmor == false)
	{
		if (randomIntValue == 1)
		{
			//PlayAnimMontage(animReboundR);
			bossPosture -= 10.f;
			montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
		}
		if (randomIntValue == 2)
		{
			//PlayAnimMontage(animReboundL);
			bossPosture -= 10.f;
			montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
		}
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::AnimReboundATK, montageLength, false);
	}
	else if (DamageEvent.DamageTypeClass==UGanpaDamageType::StaticClass())
	{
		MyDebug("animStanceCounter");
		// PlayAnimMontage(animStanceCounter);
		currentHP -= bossTakenDamage * 5.f;
		bossPosture -= 10.f;
		bossArmor -= 1.f;
		montageLength = PlayAnimMontage(animStanceCounter, 1) / (1 * animStanceCounter->RateScale);
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
	}
	else
	{
		if ((asBossAnim->Montage_IsPlaying(animNormalATKR01) || asBossAnim->Montage_IsPlaying(animNormalATKL01)) && bIsSuperArmor == false)
		{
			//TODO: 노말어택 1, 2타 에서는 패링이 발동 안되는 문제
			if (parryCount == 0)
			{
				//PlayAnimMontage(animParryR);
				attackCount += 1;
				parryCount += 1;
				montageLength = PlayAnimMontage(animParryR, 1) / (1 * animParryR->RateScale);
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
			else
			{
				//PlayAnimMontage(animParryL);
				attackCount += 1;
				parryCount = 0;
				montageLength = PlayAnimMontage(animParryL, 1) / (1 * animParryL->RateScale);
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
		}
		else
			currentHP -= bossTakenDamage;
	}
	
	return DamageAmount;
}

float ABoss::DistanceBossToPlayer()
{
	float distanceValue = GetDistanceTo(playerPawn);
	return distanceValue;
}

void ABoss::PostureRecovery()
{
	if (bTakeDamage == false)
	{
		float hpPercent = currentHP / maxHP;
		float recoveryValue = 1.f;
		float result = recoveryValue * hpPercent;
		bossPosture += result;
		bossPosture = UKismetMathLibrary::FClamp(bossPosture, 0.f, bossMaxPosture);
	}
}

