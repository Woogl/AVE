// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "BossAnimInstance.h"
#include "BossFSMComponent.h"
#include "CombatComponent.h"
#include "GanpaDamageType.h"
#include "MovieScene.h"
#include "ParryDamageType.h"
#include "PlayerCharacter.h"
#include "VectorTypes.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
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
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("attackCount %d"), attackCount));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("TakeDamage %d"), bTakeDamage));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Emerald, FString::Printf(TEXT("bIsSuperArmor %d"), bIsSuperArmor));
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::Printf(TEXT("ElectricEnergy %f"), currentElectricEnergy));
	
	if (asBossAnim->IsAnyMontagePlaying() == false)
	{
		//GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::PostureRecovery, 0.3f, false);
		PostureRecovery();
	}

	if (bossFSMComp->bIsSecondPhase == true)
	{
		ElectricEnergyRecovery();
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

void ABoss::AnimDashATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		bIsSuperArmor = true;
		montageLength = PlayAnimMontage(animDashATK01, 1) / (1 * animDashATK01->RateScale);
	}
	if (randomIntValue == 2)
	{
		bIsSuperArmor = true;
		montageLength = PlayAnimMontage(animDashATK02, 1) / (1 * animDashATK02->RateScale);
	}
}

void ABoss::AnimNormalATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		attackCount += 1;
		montageLength = PlayAnimMontage(animNormalATKR01, 1) / (1 * animNormalATKR01->RateScale);
	}
	if (randomIntValue == 2)
	{
		attackCount += 1;
		montageLength = PlayAnimMontage(animNormalATKL01, 1) / (1 * animNormalATKL01->RateScale);
	}
}

void ABoss::AnimJumpATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animJumpATK, 1) / (1 * animJumpATK->RateScale);
}

void ABoss::AnimStanceATK()
{
	bIsSuperArmor = true;
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		montageLength = PlayAnimMontage(animStanceToStab, 1) / (1 * animStanceToStab->RateScale);
	}
	if (randomIntValue == 2)
	{
		montageLength = PlayAnimMontage(animStanceToLowATK, 1) / (1 * animStanceToLowATK->RateScale);
	}
}

void ABoss::AnimBehindATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animBehindATK, 1) / (1 * animBehindATK->RateScale);
}

void ABoss::AnimBackStep()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animBackStep, 1) / (1 * animBackStep->RateScale);
}

void ABoss::AnimBladeRangeATK()
{
	bIsSuperArmor = true;
	weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 5.f, 1.f));
	montageLength = PlayAnimMontage(animBladeRangeATK, 1) / (1 * animBladeRangeATK->RateScale);
}

void ABoss::AnimGrabATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animGrabATK, 1) / (1 * animGrabATK->RateScale);
}

void ABoss::AnimSlashATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animSlashATK, 1) / (1 * animSlashATK->RateScale);
}

void ABoss::AnimComboATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animComboATK, 1) / (1 * animComboATK->RateScale);
}

void ABoss::AnimWarCry()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animWarCry, 1) / (1 * animWarCry->RateScale);
}

void ABoss::AnimFallDown()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animFallDown, 1) / (1 * animFallDown->RateScale);
}

void ABoss::AnimLaserRangeATK()
{
	bIsSuperArmor = true;
	montageLength = PlayAnimMontage(animLaserRangeATK, 1) / (1 * animLaserRangeATK->RateScale);
	GetWorldTimerManager().SetTimer(laserATKHandle, this, &ABoss::SetFocusPlayerInplace, 0.1f, true);
	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ClearFocus, montageLength, false);
}

void ABoss::AnimLightningATK()
{
	bIsSuperArmor = true;
	
	//RandomInt(1, 2);
	// if (randomIntValue == 1)
	// {
	montageLength = PlayAnimMontage(animLightningATK01, 1) / (1 * animLightningATK01->RateScale);
	// }
	// if (randomIntValue == 2)
	// {
	// 	montageLength = PlayAnimMontage(animLightningATK02, 1) / (1 * animLightningATK02->RateScale);
	// }
	currentElectricEnergy = 100;
}

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
			if (asBossAnim->yaw > 135 && DistanceBossToPlayer() > 250)
				PlayAnimMontage(bossTurnR180);
			else
				PlayAnimMontage(bossTurnR90);
		}
		else if (asBossAnim->yaw < -80)
		{
			if (asBossAnim->yaw < -135 && DistanceBossToPlayer() > 250)
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
	
	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, lineStartLoc, lineEndLoc, AttackTrace,
		false, IgnoreList, EDrawDebugTrace::ForDuration, outHit, true, FLinearColor::Red);
	if (bHit)
	{
		EnemyTarget = outHit.GetActor();
		UGameplayStatics::ApplyPointDamage(EnemyTarget, 1.f, EnemyTarget->GetActorLocation(),
			outHit, instigator, this, UStandardDamageType::StaticClass());
	}
}

void ABoss::TakeDamageFalse()
{
	bTakeDamage = false;
}

void ABoss::ClearFocus()
{
	GetWorldTimerManager().ClearTimer(laserATKHandle);
}

void ABoss::ReturnToMove()
{
	bossFSMComp->bossStates = EBossState::Move;
	bossFSMComp->bHasExecuted = false;
}

void ABoss::ReturnToWalk()
{
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
	else
	{
		if (asBossAnim->IsAnyMontagePlaying() == false)
		{
			if (randomIntValue == 1)
			{
				montageLength = PlayAnimMontage(animReboundATKL, 1) / (1 * animReboundATKL->RateScale);
				attackCount += 1;
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
			if (randomIntValue == 2)
			{
				montageLength = PlayAnimMontage(animReboundATKR, 1) / (1 * animReboundATKR->RateScale);
				attackCount += 1;
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
		}
	}
}

float ABoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float bossTakenDamage = DamageAmount;
	bTakeDamage = true;
	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::TakeDamageFalse, 1.f, false);

	
	if (DamageEvent.DamageTypeClass == UParryDamageType::StaticClass() && bIsSuperArmor == false) 
	{
		// 패리 데미지가 들어왔을 때 튕기는 모션
		if (randomIntValue == 1)
		{
			bossPosture -= 10.f;
			montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
		}
		if (randomIntValue == 2)
		{
			bossPosture -= 10.f;
			montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
		}
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::AnimReboundATK, montageLength, false);
	}
	else if (DamageEvent.DamageTypeClass==ULightningDamageType::StaticClass())
	{
		currentHP -= 10.f;
		bossPosture -= 10.f;
		montageLength = PlayAnimMontage(animLightningGroggy, 1) / (1 * animLightningGroggy->RateScale);
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
	}
	else if (DamageEvent.DamageTypeClass==UGanpaDamageType::StaticClass()) 
	{
		// 간파 데미지가 들어왔을 때 당하는 모션
		currentHP -= 10.f;
		bossPosture -= 10.f;
		montageLength = PlayAnimMontage(animStanceCounter, 1) / (1 * animStanceCounter->RateScale);
		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
	}
	else
	{
		if (bIsSuperArmor == false)
		{
			// 일반 데미지가 들어왔을 때 슈퍼아머 상태가 아니면 보스가 패리하는 모션
			if (parryCount == 0)
			{
				attackCount += 1;
				parryCount += 1;
				montageLength = PlayAnimMontage(animParryR, 1) / (1 * animParryR->RateScale);
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
			else
			{
				attackCount += 1;
				parryCount = 0;
				montageLength = PlayAnimMontage(animParryL, 1) / (1 * animParryL->RateScale);
				GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, montageLength, false);
			}
		}
		else
			currentHP -= bossTakenDamage; // 슈퍼아머 상태이면 데미지 입음
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
		float recoveryValue = 0.5f;
		float result = recoveryValue * hpPercent;
		bossPosture += result;
		bossPosture = UKismetMathLibrary::FClamp(bossPosture, 0.f, bossMaxPosture);
	}
}

void ABoss::ElectricEnergyRecovery()
{
	currentElectricEnergy += 0.1f;
	currentElectricEnergy = UKismetMathLibrary::FClamp(currentElectricEnergy, 1.f, maxElectricEnergy);
}
