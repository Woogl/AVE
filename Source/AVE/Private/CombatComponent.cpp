// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <NiagaraFunctionLibrary.h>
#include "MeshSlicer.h"
#include <GameFramework/Character.h>

UCombatComponent::UCombatComponent()
{

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::SetupWeapon(UMeshComponent* WeaponMesh)
{
	MainWeapon = WeaponMesh;
	SocketNames = MainWeapon->GetAllSocketNames();
}

void UCombatComponent::AttackCheckBegin()
{
	// ���� �� ��� �ʱ�ȭ
	AlreadyHitActors.Reset();
	CurSocketLocations.Reset();
	LastSocketLocations.Reset();

	// ���� �ʱ� ��ġ ����
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations.Emplace(FVector(0.f));
		LastSocketLocations.Emplace(MainWeapon->GetSocketLocation(SocketNames[i]));
	}
}

void UCombatComponent::AttackCheckTick()
{
	// Ʈ���̽� ����� ����
	FHitResult hit;

	// ������ ��� ���Ͽ��� Ʈ���̽�
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations[i] = MainWeapon->GetSocketLocation(SocketNames[i]);

		// ���� �����Ӻ��� ������� Ʈ���̽� (���� ��ġ)
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(hit, LastSocketLocations[i], CurSocketLocations[i], AttackChannel);

		if (bSuccess == true)
		{
			OnAttackSucceed(hit);
		}
	}

	// ���� Tick������ ������ end���� �̾ Ʈ���̽��ϵ���
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		LastSocketLocations[i] = CurSocketLocations[i];
	}
}

void UCombatComponent::SetDamageInfo(float InBaseDamage, TSubclassOf<UDamageType> InDamageType)
{
	BaseDamage = InBaseDamage;
	DamageType = InDamageType;
}

void UCombatComponent::OnAttackSucceed(FHitResult HitInfo)
{
	// �ߺ� Ÿ�� ����
	AActor* hitActor = HitInfo.GetActor();
	// �̹� ���� �������� üũ
	if (AlreadyHitActors.Contains(hitActor) == false)
	{
		// ���� ���� ���͸� �߰�
		AlreadyHitActors.Emplace(hitActor);

		// ECC_Destructible�̸� Mesh Slicer ����
		if (bEnableSlice == true && HitInfo.Component->GetCollisionObjectType() == ECC_Destructible)
		{
			SpawnMeshSlicer(HitInfo);
		}

		// ĳ���Ϳ��� ����� ���ϱ�
		if (hitActor && hitActor->IsA(ACharacter::StaticClass()))
		{
			DealDamage(hitActor);
		}

		// ������ �߻�
		if (HitstopTime > 0.f)
		{
			StartHitstop(HitstopTime);
		}

		// Ÿ�� VFX �߻�
		PlayHitFX(HitInfo);
	}
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// ���� ��
	FVector hitFromLocation = GetOwner()->GetActorLocation();
	// �߰� ����
	FHitResult hitinfo;
	// ������ ��Ʈ�ѷ�
	AController* instigator = GetOwner()->GetInstigatorController();
	// ������
	AActor* causer = GetOwner();

	UGameplayStatics::ApplyPointDamage(Target, BaseDamage, hitFromLocation, hitinfo, instigator, causer, DamageType);
}

void UCombatComponent::StartHitstop(float Time)
{
	GetOwner()->CustomTimeDilation = 0.0625f;
	GetOwner()->GetWorldTimerManager().SetTimer(HitstopTimer, this, &UCombatComponent::EndHitStop, Time, false);
}

void UCombatComponent::EndHitStop()
{
	GetOwner()->CustomTimeDilation = 1.f;
}

void UCombatComponent::PlayHitFX(FHitResult HitInfo)
{
	// Ÿ�� VFX
	if (HitNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, HitInfo.Location, HitInfo.ImpactNormal.Rotation());
	}

	// Ÿ�� SFX
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitInfo.ImpactPoint);
	}
}

void UCombatComponent::SpawnMeshSlicer(FHitResult HitInfo)
{
	FActorSpawnParameters spawnParams;
	FVector spawnLoc = HitInfo.ImpactPoint;
	FRotator spawnRot = MainWeapon->GetComponentRotation();
	GetWorld()->SpawnActor<AMeshSlicer>(AMeshSlicer::StaticClass(), spawnLoc, spawnRot, spawnParams);
}

