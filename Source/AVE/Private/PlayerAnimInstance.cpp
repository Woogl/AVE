// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "PlayerCharacter.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	bIsBlocking = false;
	Velocity = FVector(0.f);
	GroundSpeed = 0.f;
	bIsFalling = false;

	// TODO : InitState 호출
	//OnMontageBlendingOut.AddDynamic()
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Character = Cast<ACharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Character)
	{
		Velocity = Character->GetVelocity();
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		Direction = CalculateDirection(Velocity, Character->GetActorRotation());	// UKismetAnimationLibrary::CalculateDirection 쓰는게 낫나?
		bIsFalling = Character->GetCharacterMovement()->IsFalling();
	}
}

void UPlayerAnimInstance::AnimNotify_StartiFrame()
{
	Character->SetCanBeDamaged(false);
}

void UPlayerAnimInstance::AnimNotify_EndiFrame()
{
	Character->SetCanBeDamaged(true);
}

void UPlayerAnimInstance::AnimNotify_EndAttack()
{
	Cast<APlayerCharacter>(Character)->bIsAttacking = false;
}
