// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "PlayerCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystem.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>

UCombatComponent::UCombatComponent()
{
	TEnumAsByte<EObjectTypeQuery> pawn = UEngineTypes::ConvertToObjectType(ECC_Pawn);
	TEnumAsByte<EObjectTypeQuery> destructible = UEngineTypes::ConvertToObjectType(ECC_Destructible);
	TEnumAsByte<EObjectTypeQuery> worldStatic = UEngineTypes::ConvertToObjectType(ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> worldDynamic = UEngineTypes::ConvertToObjectType(ECC_WorldDynamic);
	ObjectTypes.Emplace(pawn);
	ObjectTypes.Emplace(destructible);
	ObjectTypes.Emplace(worldStatic);
	ObjectTypes.Emplace(worldDynamic);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::SetupWeapon(UStaticMeshComponent* WeaponMesh)
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
		LastSocketLocations.Emplace(MainWeapon->GetSocketLocation(SocketNames[i]));
		CurSocketLocations.SetNum(SocketNames.Num());
	}
}

void UCombatComponent::AttackCheckTick()
{
	// Ʈ���̽� ����� ����
	TArray<FHitResult> hits;

	// ������ ��� ���Ͽ��� Ʈ���̽�
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations[i] = MainWeapon->GetSocketLocation(SocketNames[i]);
		// ���� �����Ӻ��� ������� Ʈ���̽� (���� ��ġ)
		bool bSuccess = UKismetSystemLibrary::LineTraceMultiForObjects(this, LastSocketLocations[i], CurSocketLocations[i], ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::ForDuration, hits, true, FLinearColor::Red, FLinearColor::Green, 0.5f);

		if (bSuccess == true)
		{
			OnAttackSucceed(hits);
		}
	}

	// ���� Tick������ ������ end���� �̾ Ʈ���̽��ϵ���
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		LastSocketLocations[i] = CurSocketLocations[i];
	}
}

void UCombatComponent::AttackCheckEnd()
{
	// �ʱ�ȭ
	AlreadyHitActors.Reset();
	CurSocketLocations.Reset();
	LastSocketLocations.Reset();
}

void UCombatComponent::SetDamageInfo(float InBaseDamage, EDamageType InDamageType)
{
	BaseDamage = InBaseDamage;
	DamageType = InDamageType;
}

void UCombatComponent::OnAttackSucceed(TArray<FHitResult> Hits)
{
	// �ߺ� Ÿ�� ����
	for (FHitResult hit : Hits)
	{
		AActor* hitActor = hit.GetActor();
		// �̹� ���� �������� üũ
		if (AlreadyHitActors.Contains(hitActor) == false)
		{
			// ���� ���� ���͸� �߰�
			AlreadyHitActors.Emplace(hitActor);

			// ECC_Destructible�̸� Mesh Slicer ����
			if (bEnableSlice == true && hit.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
				if (player)
				{
					player->SpawnMeshSlicer();
				}
			}

			// ĳ���Ϳ��� ����� ���ϱ�
			if (hitActor->IsA(ACharacter::StaticClass()))
			{
				DealDamage(hitActor);
			}

			// ������ �߻�
			if (HitstopTime > 0.f)
			{
				StartHitstop(HitstopTime);
			}
			
			// Ÿ�� VFX �߻�
			PlayHitFX(hit);
		}
	}
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// ���� ��
	FVector hitFromLocation = GetOwner()->GetActorLocation();
	// �߰� ����
	FHitResult hitinfo;
	hitinfo.Item = (int32)DamageType;	// 0=Standard, 1=KnockBack, 2=KnockDown, 3=KnockUp, 4=NoReaction
	// ������ ��Ʈ�ѷ�
	AController* instigator = GetOwner()->GetInstigatorController();
	// ������
	AActor* causer = GetOwner();

	UGameplayStatics::ApplyPointDamage(Target, BaseDamage, hitFromLocation, hitinfo, instigator, causer, UDamageType::StaticClass());
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


