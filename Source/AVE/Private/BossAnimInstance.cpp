// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"

#include "Boss.h"
#include "KismetAnimationLibrary.h"
#include "KnockBackDamageType.h"
#include "PlayerCharacter.h"
#include "StandardDamageType.h"
#include "Components/SphereComponent.h"
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
		FRotator bossLookAtPlayer = UKismetMathLibrary::FindLookAtRotation(asBoss->GetActorLocation(), playerPawn->GetActorLocation());
		
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
}

void UBossAnimInstance::AnimNotify_WarCryEnd()
{
	asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
}

void UBossAnimInstance::WarCryRadialATK()
{
	TArray<AActor*> IgnoreList; // IgnoreList가 필수로 있어야 해서 
	IgnoreList.Add(asBoss); // 자기자신을 제외
	
	if(UGameplayStatics::ApplyRadialDamage(GetWorld(), 20, asBoss->GetActorLocation(), 500.f, UKnockBackDamageType::StaticClass(), IgnoreList, asBoss))
	{
		asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
	}
}

