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
	// 때린 적 목록 초기화
	AlreadyHitActors.Empty();
}

void UCombatComponent::AttackCheckTick()
{
	// 트레이스 결과를 저장
	TArray<FHitResult> hits;
	// 트레이스 범위
	FVector start = MainWeapon->GetSocketLocation(StartPoint);
	FVector end = MainWeapon->GetSocketLocation(EndPoint);

	// 찾을 오브젝트 타입 = Pawn, Destructible
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TEnumAsByte<EObjectTypeQuery> pawn = UEngineTypes::ConvertToObjectType(ECC_Pawn);
	TEnumAsByte<EObjectTypeQuery> destructible = UEngineTypes::ConvertToObjectType(ECC_Destructible);
	objectTypes.Add(pawn);
	objectTypes.Add(destructible);
	// 무시할 오브젝트 타입 = 없음
	TArray< AActor* > actorsToIgnore;

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, start, end, WeaponThickness, objectTypes, false, actorsToIgnore,
		EDrawDebugTrace::ForDuration, hits, true, FLinearColor::Red, FLinearColor::Green, 0.5f);

	// 중복 타격 방지
	for (auto hit : hits)
	{
		auto hitActor = hit.GetActor();
		// 이미 때린 액터인지 체크
		if (!AlreadyHitActors.Contains(hitActor))
		{
			// 새로 때린 액터만 추가
			AlreadyHitActors.Add(hitActor);

			// 역경직 발생
			if (HitstopTime > 0.f)
			{
				StartHitstop(HitstopTime);
			}

			// ECC_Destructible이면 Mesh Slicer 스폰
			if (bEnableSlice == true && hit.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				auto player = Cast<APlayerCharacter>(GetOwner());
				if (player)
				{
					player->SpawnMeshSlicer();
				}
				continue;
			}

			// 대미지 가하기
			DealDamage(hitActor);
		}
	}
}

void UCombatComponent::AttackCheckEnd()
{
	// 때린 적 목록 초기화
	AlreadyHitActors.Empty();
}

void UCombatComponent::SetDamageInfo(float InBaseDamage, EDamageType InDamageType)
{
	BaseDamage = InBaseDamage;
	DamageType = InDamageType;
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// 디버그
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("DealDamage"));

	// 때린 곳
	FVector hitFromLocation = Target->GetActorLocation();
	// 추가 정보
	FHitResult hitinfo;
	hitinfo.Item = (int32)DamageType;	// 0=Standard, 1=KnockDown, 2=KnockDown, 3=KnockUp, 4=NoReaction
	// 가해자 컨트롤러
	AController* instigator = GetOwner()->GetInstigatorController();
	// 유발자
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



