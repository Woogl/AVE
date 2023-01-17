// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "PlayerCharacter.h"
#include <Kismet/GameplayStatics.h>

UCombatComponent::UCombatComponent()
{

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::SetupWeapon(UStaticMeshComponent* WeaponMesh, float InWeaponThickness)
{
	MainWeapon = WeaponMesh;
	WeaponThickness = InWeaponThickness;
}

void UCombatComponent::AttackCheckBegin()
{
	// ���� �� ��� �ʱ�ȭ
	AlreadyHitActors.Empty();
}

void UCombatComponent::AttackCheckTick()
{
	// Ʈ���̽� ����� ����
	TArray<FHitResult> hits;
	// Ʈ���̽� ����
	FVector start = MainWeapon->GetSocketLocation(StartPoint);
	FVector end = MainWeapon->GetSocketLocation(EndPoint);

	// ã�� ������Ʈ Ÿ�� = Pawn, Destructible
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TEnumAsByte<EObjectTypeQuery> pawn = UEngineTypes::ConvertToObjectType(ECC_Pawn);
	TEnumAsByte<EObjectTypeQuery> destructible = UEngineTypes::ConvertToObjectType(ECC_Destructible);
	objectTypes.Add(pawn);
	objectTypes.Add(destructible);
	// ������ ������Ʈ Ÿ�� = ����
	TArray< AActor* > actorsToIgnore;

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, start, end, WeaponThickness, objectTypes, false, actorsToIgnore,
		EDrawDebugTrace::ForDuration, hits, true, FLinearColor::Red, FLinearColor::Green, 0.5f);

	// �ߺ� Ÿ�� ����
	for (auto hit : hits)
	{
		auto hitActor = hit.GetActor();
		// �̹� ���� �������� üũ
		if (!AlreadyHitActors.Contains(hitActor))
		{
			// ���� ���� ���͸� �߰�
			AlreadyHitActors.Add(hitActor);

			// ������ �߻�
			if (HitstopTime > 0.f)
			{
				StartHitstop(HitstopTime);
			}

			// ECC_Destructible�̸� Mesh Slicer ����
			if (bEnableSlice == true && hit.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				auto player = Cast<APlayerCharacter>(GetOwner());
				if (player)
				{
					player->SpawnMeshSlicer();
				}
				continue;
			}

			// ����� ���ϱ�
			DealDamage(hitActor);
		}
	}
}

void UCombatComponent::AttackCheckEnd()
{
	// ���� �� ��� �ʱ�ȭ
	AlreadyHitActors.Empty();
}

void UCombatComponent::SetDamageInfo(float InBaseDamage, EDamageType InDamageType)
{
	BaseDamage = InBaseDamage;
	DamageType = InDamageType;
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// �����
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("DealDamage"));

	// ���� ��
	FVector hitFromLocation = Target->GetActorLocation();
	// �߰� ����
	FHitResult hitinfo;
	hitinfo.Item = (int32)DamageType;	// 0=Standard, 1=KnockDown, 2=KnockDown, 3=KnockUp, 4=NoReaction
	// ������ ��Ʈ�ѷ�
	AController* instigator = GetOwner()->GetInstigatorController();
	// ������
	AActor* causer = GetOwner();

	UGameplayStatics::ApplyPointDamage(Target, BaseDamage, hitFromLocation, hitinfo, instigator, causer, UDamageType::StaticClass());
}

void UCombatComponent::StartHitstop(float Time)
{
	GetOwner()->CustomTimeDilation = 0.1f;
	GetOwner()->GetWorldTimerManager().SetTimer(HitstopTimer, this, &UCombatComponent::EndHitStop, Time, false);
}

void UCombatComponent::EndHitStop()
{
	GetOwner()->CustomTimeDilation = 1.f;
}



