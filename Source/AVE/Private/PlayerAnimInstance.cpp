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
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Player = Cast<APlayerCharacter>(TryGetPawnOwner());
//	OnMontageBlendingOut.AddDynamic(this,&UPlayerAnimInstance::InitPlayerState);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Player)
	{
		Velocity = Player->GetVelocity();
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		Direction = CalculateDirection(Velocity, Player->GetActorRotation());	// UKismetAnimationLibrary::CalculateDirection ¾²´Â°Ô ³´³ª?
		bIsFalling = Player->GetCharacterMovement()->IsFalling();
	}
}

void UPlayerAnimInstance::AnimNotify_StartiFrame()
{
	Player->SetCanBeDamaged(false);
}

void UPlayerAnimInstance::AnimNotify_EndiFrame()
{
	Player->SetCanBeDamaged(true);
}

void UPlayerAnimInstance::AnimNotify_EndAttack()
{
	Player->EndAttack();
}	

void UPlayerAnimInstance::AnimNotify_EndSkill()
{
	Player->EndSkill();
}

void UPlayerAnimInstance::InitPlayerState() {
	Player->InitState();
}