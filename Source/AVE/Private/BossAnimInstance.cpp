// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAnimInstance.h"

#include "Boss.h"
#include "KismetAnimationLibrary.h"
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
	asBoss->currentElectricEnergy = 1.f;
	if (bDoOnce == false)
	{
		asBoss->bossPosture = 100;
		bDoOnce = true;
	}
}

void UBossAnimInstance::AnimNotify_WarCryEnd()
{
	asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
}

void UBossAnimInstance::AnimNotify_OnSuperArmor()
{
	asBoss->bIsSuperArmor = true;
}

void UBossAnimInstance::AnimNotify_OffSuperArmor()
{
	asBoss->bIsSuperArmor = false;
}

void UBossAnimInstance::AnimNotify_LightningUp()
{
	asBoss->LaunchCharacter(FVector(0, 0, lightningVelocityZ), false, false);
	asBoss->weaponMeshSubComp->SetRelativeScale3D(FVector(1.f, 8.f, 1.f));
}

void UBossAnimInstance::AnimNotify_LightningDown()
{
	asBoss->LaunchCharacter(FVector(0, 0, lightningVelocityMZ), false, false);
}

void UBossAnimInstance::WarCryRadialATK()
{
	TArray<AActor*> IgnoreList; // IgnoreList가 필수로 있어야 해서 
	IgnoreList.Add(asBoss); // 자기자신을 제외
	
	if(UGameplayStatics::ApplyRadialDamage(GetWorld(), 20, asBoss->GetActorLocation(), 500.f, UUnguardableDamageType::StaticClass(), IgnoreList, asBoss))
	{
		asBoss->GetWorldTimerManager().ClearTimer(warCryTimerHandle);
	}
}

