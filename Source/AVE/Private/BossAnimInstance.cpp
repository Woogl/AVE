// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"

#include "Boss.h"
#include "KismetAnimationLibrary.h"
#include "LightningDamageType.h"
#include "LowAttackDamageType.h"
#include "PlayerCharacter.h"
#include "UnguardableDamageType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UBossAnimInstance::UBossAnimInstance()
{
	velocity = FVector(0.f);
	speed = 0.f;
	bIsFalling = false;
	pitch = 0.f;
	yaw = 0.f;
}

void UBossAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	asBoss = Cast<ABoss>(TryGetPawnOwner());
	playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	asPlayer = Cast<APlayerCharacter>(playerPawn);
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (asBoss)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("IfSuccecss"));
		FRotator bossLookAtPlayer = UKismetMathLibrary::FindLookAtRotation(
			asBoss->GetActorLocation(), playerPawn->GetActorLocation());

		velocity = asBoss->GetVelocity();
		speed = UKismetMathLibrary::VSizeXY(velocity);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Speed %f"), speed));
		direction = UKismetAnimationLibrary::CalculateDirection(velocity, asBoss->GetActorRotation());
		bIsFalling = asBoss->GetCharacterMovement()->IsFalling();
		FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(bossLookAtPlayer, asBoss->GetActorRotation());
		pitch = deltaRot.Pitch;
		yaw = deltaRot.Yaw;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Yaw %f"), yaw));
	}
}

void UBossAnimInstance::AnimNotify_WarCryBegin()
{
	asBoss->GetWorldTimerManager().SetTimer(warCryTimerHandle, this, &UBossAnimInstance::WarCryRadialATK, 0.1f, true);
	asBoss->currentElectricEnergy = 1.f;
	if (bDoOnce == false)
	{
		asBoss->bossPosture = 100;
		asBoss->currentHP = asBoss->maxHP;
		bDoOnce = true;
	}
}

void UBossAnimInstance::AnimNotify_WarCryEnd()
{
	asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
}

void UBossAnimInstance::WarCryRadialATK()
{
	TArray<AActor*> IgnoreList; // IgnoreList가 필수로 있어야 해서 
	IgnoreList.Add(asBoss); // 자기자신을 제외

	if (UGameplayStatics::ApplyRadialDamage(GetWorld(), 20, asBoss->GetActorLocation(), 500.f,
	                                        UUnguardableDamageType::StaticClass(), IgnoreList, asBoss))
	{
		asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
	}
}

void UBossAnimInstance::AnimNotify_OnSuperArmor()
{
	asBoss->bIsSuperArmor = true;
}

void UBossAnimInstance::AnimNotify_OffSuperArmor()
{
	asBoss->bIsSuperArmor = false;
}

void UBossAnimInstance::AnimNotify_LightningBegin()
{
	asBoss->GetWorldTimerManager().SetTimer(ligntningTimerHandle, this,
	                                        &UBossAnimInstance::AnimNotify_LightningRadialATK, 0.1f, true);
}

void UBossAnimInstance::AnimNotify_LightningEnd()
{
	asBoss->GetWorldTimerManager().ClearTimer(ligntningTimerHandle);
}

void UBossAnimInstance::AnimNotify_LightningRadialATK()
{
	TArray<AActor*> IgnoreList; // IgnoreList가 필수로 있어야 해서 
	IgnoreList.Add(asBoss); // 자기자신을 제외
	FVector originLoc = asBoss->GetActorLocation() + (asBoss->GetActorForwardVector() * 100.f);
	if (UGameplayStatics::ApplyRadialDamage(GetWorld(), 20, originLoc, 1000.f, ULightningDamageType::StaticClass(),
	                                        IgnoreList, asBoss))
	{
		asBoss->GetWorldTimerManager().ClearTimer(ligntningTimerHandle);
	}
}

void UBossAnimInstance::AnimNotify_FlyingTrue()
{
	asBoss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UBossAnimInstance::AnimNotify_FlyingFalse()
{
	asBoss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UBossAnimInstance::AnimNotify_BladeRangeBegin()
{
	asBoss->GetWorldTimerManager().SetTimer(bladeRangeTimerHandle, this,
	                                        &UBossAnimInstance::AnimNotify_BladeRangeRadialATK, 0.02f, true);
}

void UBossAnimInstance::AnimNotify_BladeRangeEnd()
{
	asBoss->GetWorldTimerManager().ClearTimer(bladeRangeTimerHandle);
}

void UBossAnimInstance::AnimNotify_BladeRangeRadialATK()
{
	TArray<AActor*> IgnoreList; // IgnoreList가 필수로 있어야 해서 
	IgnoreList.Add(asBoss); // 자기자신을 제외

	if (UGameplayStatics::ApplyRadialDamage(GetWorld(), 20, asBoss->GetActorLocation(), 700.f,
	                                        ULowAttackDamageType::StaticClass(), IgnoreList, asBoss))
	{
		asBoss->GetWorldTimerManager().ClearTimer(bladeRangeTimerHandle);
	}
}

void UBossAnimInstance::AnimNotify_PatternATK()
{
	if (asBoss->attackCount >=0)
	{
		if (asBoss->attackCount == 2)
		{
			asBoss->bossFSMComp->bossStates = EBossState::JumpATK;
			bParry = true;
			asBoss->attackCount += 1;
			asBoss->bossFSMComp->bHasExecuted = false;
		}
		if (asBoss->attackCount == 4)
		{
			asBoss->bossFSMComp->bossStates = EBossState::ComboATK;
			bParry = true;
			asBoss->attackCount += 1;
			asBoss->bossFSMComp->bHasExecuted = false;
		}
		if (asBoss->attackCount == 7)
		{
			asBoss->bossFSMComp->bossStates = EBossState::GrabATK;
			bParry = true;
			asBoss->attackCount += 1;
			asBoss->bossFSMComp->bHasExecuted = false;
		}
		if (asBoss->attackCount >= 9)
		{
			asBoss->bossFSMComp->bossStates = EBossState::BackStep;
			bParry = true;
			asBoss->attackCount = 0;
			asBoss->bossFSMComp->bHasExecuted = false;
		}
	}
	else
	{
		asBoss->bossFSMComp->bossStates = EBossState::Move;
		asBoss->bossFSMComp->bHasExecuted = false;
	}
	
}

void UBossAnimInstance::AnimNotify_ReturnToMove()
{
	asBoss->bossFSMComp->bossStates = EBossState::Move;
	asBoss->bossFSMComp->bHasExecuted = false;
	bParry = false;
}
