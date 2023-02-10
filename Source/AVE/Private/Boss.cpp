// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "BossAnimInstance.h"
#include "BossFSMComponent.h"
#include "CombatComponent.h"
#include "MovieScene.h"
#include "PlayerCharacter.h"
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
}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::SetZeroSpeed()
{
	float decreaseWalkSpeed = GetCharacterMovement()->GetMaxSpeed() - 10.f;
	float walkSpeedClamp = UKismetMathLibrary::FClamp(decreaseWalkSpeed, 0.f, 500.f);
	GetCharacterMovement()->MaxWalkSpeed = walkSpeedClamp;
}

// void ABoss::BossPlayMontageSA(UAnimMontage* selectMontage)
// {
// 	bIsSuperArmor = true;
// 	GetMesh()->GetAnimInstance()->Montage_Play(selectMontage);
// 	montageLength = EMontagePlayReturnType::MontageLength(selectMontage);
// 	if (DistanceBossToPlayer() > 400)
// 	{
// 		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToWalk, 1.f, false, montageLength);
// 	}
// 	else
// 		GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, 1.f, false, montageLength);
// }

void ABoss::AnimDashATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		bIsSuperArmor = true;
		PlayAnimMontage(animDashATK01);
		montageLength = PlayAnimMontage(animDashATK01, 1) / (1 * animDashATK01->RateScale);
	}
	if (randomIntValue == 2)
	{
		bIsSuperArmor = true;
		PlayAnimMontage(animDashATK02);
		montageLength = PlayAnimMontage(animDashATK02, 1) / (1 * animDashATK02->RateScale);
	}
}

void ABoss::AnimNormalATK()
{
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		PlayAnimMontage(animNormalATKR01);
		attackCount++;
		montageLength = PlayAnimMontage(animNormalATKR01, 1) / (1 * animNormalATKR01->RateScale);
	}
	if (randomIntValue == 2)
	{
		PlayAnimMontage(animNormalATKL01);
		attackCount++;
		montageLength = PlayAnimMontage(animNormalATKL01, 1) / (1 * animNormalATKL01->RateScale);
	}
}

void ABoss::AnimJumpATK()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animJumpATK);
	montageLength = PlayAnimMontage(animJumpATK, 1) / (1 * animNormalATKL01->RateScale);
}

void ABoss::AnimStanceATK()
{
	bIsSuperArmor = true;
	RandomInt(1, 2);
	if (randomIntValue == 1)
	{
		PlayAnimMontage(animStanceToStab);
		montageLength = PlayAnimMontage(animStanceToStab, 1) / (1 * animStanceToStab->RateScale);
	}
	if (randomIntValue == 2)
	{
		PlayAnimMontage(animStanceToLowATK);
		montageLength = PlayAnimMontage(animStanceToLowATK, 1) / (1 * animStanceToLowATK->RateScale);
	}
}

void ABoss::AnimBehindATK()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animBehindATK);
	montageLength = PlayAnimMontage(animBehindATK, 1) / (1 * animBehindATK->RateScale);
}

void ABoss::AnimBackStep()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animBackstep);
	montageLength = PlayAnimMontage(animBackstep, 1) / (1 * animBackstep->RateScale);
}

void ABoss::AnimBladeRangeATK()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animBladeRangeATK);
	weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 5.f, 1.f));
	montageLength = PlayAnimMontage(animBladeRangeATK, 1) / (1 * animBladeRangeATK->RateScale);
}

void ABoss::AnimATK02()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animATK02);
	montageLength = PlayAnimMontage(animATK02, 1) / (1 * animATK02->RateScale);
}

void ABoss::AnimATK03()
{
	bIsSuperArmor = true;
	PlayAnimMontage(animATK03);
	montageLength = PlayAnimMontage(animATK03, 1) / (1 * animATK03->RateScale);
}

// void ABoss::BossPlayMontage(UAnimMontage* selectMontage)
// {
// 	PlayAnimMontage(selectMontage);
// 	montageLength = PlayAnimMontage(selectMontage, 1) / (1 * selectMontage->RateScale);
// 	//montageLength = EMontagePlayReturnType::MontageLength(selectMontage);
// 	// if (DistanceBossToPlayer() > 400)
// 	// {
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToWalk, 1.f, false, montageLength);
// 	// }
// 	// else if (bossFSMComp->bossStates == EBossState::Backstep)
// 	// {
// 	// 	if (bossFSMComp->randomIntValue)
// 	// 	{
// 	// 		
// 	// 	}
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToBladeRangeATK, 1.f, false, montageLength);
// 	// }
// 	// else
// 	// 	GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, 1.f, false, montageLength);
// }
//
// void ABoss::ReturnToMove()
// {
// 	bIsSuperArmor = false;
// 	bossFSMComp->bossStates = EBossState::Move;
// 	bossFSMComp->seqValue = 0;
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

void ABoss::SetFocusPlayerTick()
{
	if (asBossAnim->speed > 100)
	{
		FRotator bossLookAtPlayer = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), playerPawn->GetActorLocation());
		FRotator setLerp = UKismetMathLibrary::RLerp(GetActorRotation(), bossLookAtPlayer, 0.1f, true);
		this->SetActorRotation(FRotator(0, setLerp.Yaw, 0));
	}
	else
	{
		AnimTurnInPlace();
	}
}

void ABoss::ReturnToMove()
{
	bIsSuperArmor = false;
	bossFSMComp->bossStates = EBossState::Move;
	bossFSMComp->seqValue = 0;
}

void ABoss::ReturnToWalk()
{
	bIsSuperArmor = false;
	bossFSMComp->bossStates = EBossState::Walk;
	bossFSMComp->seqValue = 0;
}

void ABoss::ReturnToBladeRangeATK()
{
	bossFSMComp->bossStates = EBossState::BladeRangeATK;
	bossFSMComp->seqValue = 0;
}

void ABoss::AnimReboundATK()
{
	if (attackCount == 2)
	{
		bossFSMComp->bossStates = EBossState::JumpATK;
		attackCount += 1;
		bossFSMComp->seqValue = 0;
	}
	else if (attackCount == 4)
	{
		bossFSMComp->bossStates = EBossState::ATK02;
		attackCount += 1;
		bossFSMComp->seqValue = 0;
	}
	else if (attackCount == 7)
	{
		bossFSMComp->bossStates = EBossState::ATK03;
		attackCount += 1;
		bossFSMComp->seqValue = 0;
	}
	else if (attackCount >= 9)
	{
		bossFSMComp->bossStates = EBossState::Backstep;
		attackCount = 0;
		bossFSMComp->seqValue = 0;
	}
	else
	{
		if (randomIntValue == 1)
		{
			PlayAnimMontage(animReboundATKL);
			montageLength = PlayAnimMontage(animReboundATKL, 1) / (1 * animReboundATKL->RateScale);
			attackCount += 1;
			GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, 1.f, false, montageLength);
		}
		if (randomIntValue == 2)
		{
			PlayAnimMontage(animReboundATKR);
			montageLength = PlayAnimMontage(animReboundATKR, 1) / (1 * animReboundATKR->RateScale);
			attackCount += 1;
			GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::ReturnToMove, 1.f, false, montageLength);
		}
	}
}

float ABoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageAmount > 1.f)
	{
		currentHP -= DamageAmount;
	}
	else
	{
		if (bIsSuperArmor == true)
		{
			bossFSMComp->ReturnToMove();
		}
		else
		{
			if (randomIntValue == 1)
			{
				PlayAnimMontage(animReboundR);
				montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
			}
			if (randomIntValue == 2)
			{
				PlayAnimMontage(animReboundL);
				montageLength = PlayAnimMontage(animReboundR, 1) / (1 * animReboundR->RateScale);
			}
			GetWorldTimerManager().SetTimer(delayHandle, this, &ABoss::AnimReboundATK, 1.f, false, montageLength);
			// GetWorldTimerManager().SetTimer(delayHandle, FTimerDelegate::CreateLambda([&]()
			// {
			// 	AnimReboundATK();
			// 	GetOwner()->GetWorldTimerManager().ClearTimer(delayHandle);
			// }), 1.f, false, montageLength);
		}
	}
	return DamageAmount;
}

float ABoss::DistanceBossToPlayer()
{
	float distanceValue = GetDistanceTo(playerPawn);
	return distanceValue;
}

