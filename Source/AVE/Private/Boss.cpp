// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"

#include "BossAnimInstance.h"
#include "BossFSMComponent.h"
#include "CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
    	weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 1.2f, 1.2f));
    }

	//Combat Component 생성
	combatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("combatComp"));

	//BossFSM Component 생성 -> BP_BossFSM 에서 블루프린트와 조합해서 사용
	//bossFSMComp = CreateDefaultSubobject<UBossFSMComponent>(TEXT("bossFSMComp"));
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	combatComp->SetupWeapon(weaponMeshSubComp);

	currentHP = maxHP;

	asBossAnim = Cast<UBossAnimInstance>(GetMesh()->GetAnimInstance());

	playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
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

void ABoss::AnimTurnInPlace()
{
	if (asBossAnim->IsAnyMontagePlaying() == false)
	{
		if (asBossAnim->yaw > 70)
		{
			if (asBossAnim->yaw > 135)
				PlayAnimMontage(bossTurnR180);
			else
				PlayAnimMontage(bossTurnR90);
		}
		else if (asBossAnim->yaw < -70)
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

float ABoss::DistanceBossToPlayer()
{
	distanceValue = GetDistanceTo(playerPawn);
	return distanceValue;
}

