// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"

#include "Boss.h"
#include "KismetAnimationLibrary.h"
#include "PlayerCharacter.h"
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
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Debug %f"), speed));
		direction = UKismetAnimationLibrary::CalculateDirection(velocity, asBoss->GetActorRotation());
		bIsFalling = asBoss->GetCharacterMovement()->IsFalling();
		FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(bossLookAtPlayer, asBoss->GetActorRotation());
		pitch = deltaRot.Pitch;
		yaw = deltaRot.Yaw;
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("Debug %f"), yaw));
	}
}
