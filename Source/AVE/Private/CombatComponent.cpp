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
	// 때린 적 목록 초기화
	AlreadyHitActors.Reset();
	CurSocketLocations.Reset();
	LastSocketLocations.Reset();

	// 소켓 초기 위치 설정
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations.Emplace(FVector(0.f));
		LastSocketLocations.Emplace(MainWeapon->GetSocketLocation(SocketNames[i]));
	}
}

void UCombatComponent::AttackCheckTick()
{
	// 트레이스 결과를 저장
	FHitResult hit;

	// 무기의 모든 소켓에서 트레이스
	for (int i = 0; i < SocketNames.Num(); i++)
	{
		CurSocketLocations[i] = MainWeapon->GetSocketLocation(SocketNames[i]);

		// 지난 프레임부터 현재까지 트레이스 (소켓 위치)
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(hit, LastSocketLocations[i], CurSocketLocations[i], AttackChannel);

		if (bSuccess == true)
		{
			OnAttackSucceed(hit);
		}
	}

	// 다음 Tick에서는 현재의 end에서 이어서 트레이스하도록
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
	// 중복 타격 방지
	AActor* hitActor = HitInfo.GetActor();
	// 이미 때린 액터인지 체크
	if (AlreadyHitActors.Contains(hitActor) == false)
	{
		// 새로 때린 액터만 추가
		AlreadyHitActors.Emplace(hitActor);

		// ECC_Destructible이면 Mesh Slicer 스폰
		if (bEnableSlice == true && HitInfo.Component->GetCollisionObjectType() == ECC_Destructible)
		{
			SpawnMeshSlicer(HitInfo);
		}

		// 캐릭터에게 대미지 가하기
		if (hitActor && hitActor->IsA(ACharacter::StaticClass()))
		{
			DealDamage(hitActor);
		}

		// 역경직 발생
		if (HitstopTime > 0.f)
		{
			StartHitstop(HitstopTime);
		}

		// 타격 VFX 발생
		PlayHitFX(HitInfo);
	}
}

void UCombatComponent::DealDamage(AActor* Target)
{
	// 때린 곳
	FVector hitFromLocation = GetOwner()->GetActorLocation();
	// 추가 정보
	FHitResult hitinfo;
	// 가해자 컨트롤러
	AController* instigator = GetOwner()->GetInstigatorController();
	// 유발자
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

void UCombatComponent::SpawnMeshSlicer(FHitResult HitInfo)
{
	FActorSpawnParameters spawnParams;
	FVector spawnLoc = HitInfo.ImpactPoint;
	FRotator spawnRot = MainWeapon->GetComponentRotation();
	GetWorld()->SpawnActor<AMeshSlicer>(AMeshSlicer::StaticClass(), spawnLoc, spawnRot, spawnParams);
}

