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
	// 때린 적 목록 초기화
	AlreadyHitActors.Reset();
	CurSocketLocations.Reset();
	LastSocketLocations.Reset();

	// 소켓 초기 위치 설정
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		LastSocketLocations.Emplace(MainWeapon->GetSocketLocation(SocketNames[i]));
		CurSocketLocations.SetNum(SocketNames.Num());
	}
}

void UCombatComponent::AttackCheckTick()
{
	// 트레이스 결과를 저장
	TArray<FHitResult> hits;

	// 무기의 모든 소켓에서 트레이스
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations[i] = MainWeapon->GetSocketLocation(SocketNames[i]);
		// 지난 프레임부터 현재까지 트레이스 (소켓 위치)
		bool bSuccess = UKismetSystemLibrary::LineTraceMultiForObjects(this, LastSocketLocations[i], CurSocketLocations[i], ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::ForDuration, hits, true, FLinearColor::Red, FLinearColor::Green, 0.5f);

		if (bSuccess == true)
		{
			OnAttackSucceed(hits);
		}
	}

	// 다음 Tick에서는 현재의 end에서 이어서 트레이스하도록
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		LastSocketLocations[i] = CurSocketLocations[i];
	}
}

void UCombatComponent::AttackCheckEnd()
{
	// 초기화
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
	// 중복 타격 방지
	for (FHitResult hit : Hits)
	{
		AActor* hitActor = hit.GetActor();
		// 이미 때린 액터인지 체크
		if (AlreadyHitActors.Contains(hitActor) == false)
		{
			// 새로 때린 액터만 추가
			AlreadyHitActors.Emplace(hitActor);

			// ECC_Destructible이면 Mesh Slicer 스폰
			if (bEnableSlice == true && hit.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
				if (player)
				{
					player->SpawnMeshSlicer();
				}
			}

			// 캐릭터에게 대미지 가하기
			if (hitActor->IsA(ACharacter::StaticClass()))
			{
				DealDamage(hitActor);
			}

			// 역경직 발생
			if (HitstopTime > 0.f)
			{
				StartHitstop(HitstopTime);
			}
			
			// 타격 VFX 발생
			PlayHitFX(hit);
		}
	}
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// 때린 곳
	FVector hitFromLocation = GetOwner()->GetActorLocation();
	// 추가 정보
	FHitResult hitinfo;
	hitinfo.Item = (int32)DamageType;	// 0=Standard, 1=KnockBack, 2=KnockDown, 3=KnockUp, 4=NoReaction
	// 가해자 컨트롤러
	AController* instigator = GetOwner()->GetInstigatorController();
	// 유발자
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
	// 타격 VFX
	if (HitNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, HitInfo.Location, HitInfo.ImpactNormal.Rotation());
	}

	// 타격 SFX
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitInfo.ImpactPoint);
	}
}


