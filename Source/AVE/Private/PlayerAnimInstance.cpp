// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>

UPlayerAnimInstance::UPlayerAnimInstance()
{
	bIsBlocking = false;
	Velocity = FVector(0.f);
	GroundSpeed = 0.f;
	bIsFalling = false;
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
		Direction = CalculateDirection(Velocity, Character->GetActorRotation());	// UKismetAnimationLibrary::CalculateDirection ¾²´Â°Ô ³´³ª?
		bIsFalling = Character->GetCharacterMovement()->IsFalling();
	}
}

void UPlayerAnimInstance::SetIsBlocking(bool Value)
{
	bIsBlocking = Value;
}