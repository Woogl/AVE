// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include <Components/CapsuleComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>

ADummy::ADummy()
{
	PrimaryActorTick.bCanEverTick = true;

	// ���������� ī�޶� �Ȱɸ���
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// ������ �ǰ� �����ϰ�
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ADummy::BeginPlay()
{
	Super::BeginPlay();
	
	// �ʱ� ü�� ����
	CurrentHealth = MaxHealth;
}

void ADummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADummy::LookAtPlayer()
{
	FVector currentLoc = GetActorLocation();
	FVector targetLoc = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	float newYaw = UKismetMathLibrary::FindLookAtRotation(currentLoc, targetLoc).Yaw;
	FRotator newRotator = FRotator(GetActorRotation().Pitch, newYaw, GetActorRotation().Roll);
	SetActorRotation(newRotator);
}

void ADummy::OnFinishered()
{
	//�����
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("OnFinishered"));

	// �÷��̾� �Ĵٺ���
	LookAtPlayer();

	// ����ũ�ٿ� �ִϸ��̼� ����
	PlayAnimMontage(Finishered[0]);
}

void ADummy::SliceBodyPart(EBodyPart BodyIndex, FVector Impulse, float RagdollDelay)
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	// �Ӹ� �ڸ� ���
	if (BodyIndex == EBodyPart::EBP_Head)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("neck_01"));
	}
	// ���� �ڸ� ���
	else if (BodyIndex == EBodyPart::EBP_LeftArm)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("upperarm_l"));
	}
	// ������ �ڸ� ���
	else if (BodyIndex == EBodyPart::EBP_RightArm)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("upperarm_r"));
	}
	// �㸮 �ڸ� ���
	else if (BodyIndex == EBodyPart::EBP_Waist)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("spine_01"));
	}
	// �޴ٸ� �ڸ� ���
	else if (BodyIndex == EBodyPart::EBP_LeftLeg)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("thigh_l"));
	}
	// �����ٸ� �ڸ� ���
	else if (BodyIndex == EBodyPart::EBP_RightLeg)
	{
		GetMesh()->BreakConstraint(Impulse, GetActorLocation(), FName("thigh_r"));
	}
	// ������ �� �ڸ� ���
	else
	{
		// �����
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid Body Part !!!"));
	}

	// �ð� ������ Ragdoll Ȱ��ȭ
	if (RagdollDelay <= 0.f)
	{
		ActivateRagdoll();
	}
	else
	{
		GetWorldTimerManager().SetTimer(RagdollTimer, this, &ADummy::ActivateRagdoll, RagdollDelay, false);
	}

	// TODO: �߶��� �� �ʿ� ���� ���� ����Ʈ �߻� ����
}

void ADummy::ActivateRagdoll()
{
	// �ݸ��� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// ���׵� Ȱ��ȭ
	GetMesh()->SetSimulatePhysics(true);
}

