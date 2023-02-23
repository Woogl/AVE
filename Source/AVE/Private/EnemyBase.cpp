// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include "CombatComponent.h"
#include <Components/CapsuleComponent.h>
#include "AIManager.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "EnemyWidget.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hp = hpMax = 100;
	posture = postureMax = 50;
	damage = 10;
	postureCool = 1;
	postureRate = 1;

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	blackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::LookAtPlayer()
{
	FVector currentLoc = GetActorLocation();
	FVector targetLoc = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	float newYaw = UKismetMathLibrary::FindLookAtRotation(currentLoc, targetLoc).Yaw;
	FRotator newRotator = FRotator(GetActorRotation().Pitch, newYaw, GetActorRotation().Roll);
	SetActorRotation(newRotator);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{/*
	hp -= characterDamage;
	if (hp < 0)
		onDie();*/

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemyBase::onActionAttack()
{
}

void AEnemyBase::onActionGuard()
{
}

void AEnemyBase::onHit()
{
	
}

void AEnemyBase::onHitCrushed()
{
}

void AEnemyBase::onDie()
{
	Destroy();
}

void AEnemyBase::OnFinishered()
{
	// 플레이어 쳐다보기
	LookAtPlayer();

	// 테이크다운 애니메이션 실행
	PlayAnimMontage(enemyDeathMontage, 1, TEXT("Finish"));
}

void AEnemyBase::SliceBodyPart(EBodyPart BodyIndex, FVector Impulse, float RagdollDelay)
{

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	// 머리 자를 경우
	if (BodyIndex == EBodyPart::EBP_Head)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("neck_01"));
	}
	// 왼팔 자를 경우
	else if (BodyIndex == EBodyPart::EBP_LeftArm)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("upperarm_l"));
	}
	// 오른팔 자를 경우
	else if (BodyIndex == EBodyPart::EBP_RightArm)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("upperarm_r"));
	}
	// 허리 자를 경우
	else if (BodyIndex == EBodyPart::EBP_Waist)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("spine_02"));
	}
	// 왼다리 자를 경우
	else if (BodyIndex == EBodyPart::EBP_LeftLeg)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("thigh_l"));
	}
	// 오른다리 자를 경우
	else if (BodyIndex == EBodyPart::EBP_RightLeg)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("thigh_r"));
	}
	// 엉뚱한 곳 자를 경우
	else
	{
		// 디버그
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid Body Part !!!"));
	}

	// 시간 지나면 Ragdoll 활성화
	if (RagdollDelay <= 0.f)
	{
		ActivateRagdoll();
	}
	else
	{
		GetWorldTimerManager().SetTimer(RagdollTimer, this, &AEnemyBase::ActivateRagdoll, RagdollDelay, false);
	}

	// TODO: 잘라진 양 쪽에 각각 전기 이펙트 발생 루프
}

void AEnemyBase::ActivateRagdoll()
{
	// 콜리전 변경
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// 래그돌 활성화
	GetMesh()->SetSimulatePhysics(true);
}

void AEnemyBase::onGetSet()
{
	blackboard->SetValueAsBool(TEXT("Armed"), true);
	blackboard->SetValueAsEnum(TEXT("AIState"), 7);
}

void AEnemyBase::onSetManager(AAIManager* Manager)
{
	myManager = Manager;
}

void AEnemyBase::UpdateMoveSpeed(float NewSpeed)
{
	this->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

float AEnemyBase::GetHP()
{
	return hp;
}

float AEnemyBase::GetPosture()
{
	return posture;
}

float AEnemyBase::GetMaxHP()
{
	return hpMax;
}

float AEnemyBase::GetMaxPosture()
{
	return postureMax;
}

void AEnemyBase::regenPosture()
{
	posture += (postureMax / 10);
	if (posture > 0)
		executionable = false;
}