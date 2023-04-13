// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "PlayerCharacter.h"
#include "AllAVEDamageTypes.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/GameplayStatics.h"

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
	// <문성훈> 몽타주 비정상 종료시(캔슬로 인한 종료) 노티파이가 콜이 안됨. 몽타주가 끝날 때 자동으로 Broadcast 되는 델리게이트에 플레이어 상태 초기화 함수 바인드
	OnMontageBlendingOut.AddDynamic(this, &UPlayerAnimInstance::InitPlayerState);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Player)
	{
		Velocity = Player->GetVelocity();
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		Direction = CalculateDirection(Velocity, Player->GetActorRotation());	// UKismetAnimationLibrary::CalculateDirection 쓰는게 낫나?
		bIsFalling = Player->GetCharacterMovement()->IsFalling();
		// Foot IK
		FootIK(DeltaSeconds);
	}
}

void UPlayerAnimInstance::FootIK(float DeltaTime)
{
	if (Player && !Player->GetCharacterMovement()->IsFalling()) // No Falling
	{
		IgnoreActors.Emplace(Player);

		TTuple<bool, float> Foot_R = CapsuleDistance("ik_foot_r", Player);
		TTuple<bool, float> Foot_L = CapsuleDistance("ik_foot_l", Player);

		if (Foot_L.Get<0>() || Foot_R.Get<0>())
		{
			const float Selectfloat = UKismetMathLibrary::SelectFloat(Foot_L.Get<1>(), Foot_R.Get<1>(), Foot_L.Get<1>() >= Foot_R.Get<1>());
			Displacement = FMath::FInterpTo(Displacement, (Selectfloat - 90.f) * -1.f, DeltaTime, IKInterpSpeed);

			TTuple<bool, float, FVector> FootTrace_R = FootLineTrace("ik_foot_r", Player);
			TTuple<bool, float, FVector> FootTrace_L = FootLineTrace("ik_foot_l", Player);

			const float Distance_R = FootTrace_R.Get<1>();
			const FVector FootRVector(FootTrace_R.Get<2>());
			const FRotator MakeRRot(UKismetMathLibrary::DegAtan2(FootRVector.X, FootRVector.Z) * -1.f, 0.f, UKismetMathLibrary::DegAtan2(FootRVector.Y, FootRVector.Z));

			RRot = FMath::RInterpTo(RRot, MakeRRot, DeltaTime, IKInterpSpeed);
			RIK = FMath::FInterpTo(RIK, (Distance_R - 110.f) / -45.f, DeltaTime, IKInterpSpeed);

			const float Distance_L = FootTrace_L.Get<1>();
			const FVector FootLVector(FootTrace_L.Get<2>());
			const FRotator MakeLRot(UKismetMathLibrary::DegAtan2(FootLVector.X, FootLVector.Z) * -1.f, 0.f, UKismetMathLibrary::DegAtan2(FootLVector.Y, FootLVector.Z));

			LRot = FMath::RInterpTo(LRot, MakeLRot, DeltaTime, IKInterpSpeed);
			LIK = FMath::FInterpTo(LIK, (Distance_L - 110.f) / -45.f, DeltaTime, IKInterpSpeed);
		}
	}
	else
	{
		LRot = FMath::RInterpTo(LRot, FRotator::ZeroRotator, DeltaTime, IKInterpSpeed);
		LIK = FMath::FInterpTo(LIK, 0.f, DeltaTime, IKInterpSpeed);

		RRot = FMath::RInterpTo(RRot, FRotator::ZeroRotator, DeltaTime, IKInterpSpeed);
		RIK = FMath::FInterpTo(RIK, 0.f, DeltaTime, IKInterpSpeed);
	}
}

TTuple<bool, float> UPlayerAnimInstance::CapsuleDistance(FName SocketName, ACharacter* Char)
{
	const FVector WorldLocation{ Char->GetMesh()->GetComponentLocation() };
	const FVector BreakVector{ WorldLocation + FVector(0.f, 0.f, 90.f) };

	const FVector SocketLocation{ Char->GetMesh()->GetSocketLocation(SocketName) };

	const FVector Start{ SocketLocation.X, SocketLocation.Y, BreakVector.Z };
	const FVector End{ Start - FVector(0.f, 0.f, 150.f) };

	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FColor::Red,
		FColor::Green,
		0.1f);

	const bool Result(HitResult.bBlockingHit);

	return MakeTuple(Result, HitResult.Distance);
}

TTuple<bool, float, FVector> UPlayerAnimInstance::FootLineTrace(FName SocketName, ACharacter* Char)
{
	const FVector SocketLocation{ Char->GetMesh()->GetSocketLocation(SocketName) };
	const FVector RootLocation(Char->GetMesh()->GetSocketLocation("root"));

	const FVector Start{ SocketLocation.X,SocketLocation.Y,RootLocation.Z };

	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start + FVector(0.f, 0.f, 105.f),
		Start + FVector(0.f, 0.f, -105.f),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FColor::Yellow,
		FColor::Blue,
		0.1f);

	const bool Result(HitResult.bBlockingHit);

	if (HitResult.bBlockingHit)
	{
		return MakeTuple(Result, HitResult.Distance, HitResult.Normal);
	}
	else
	{
		return MakeTuple(Result, 999.f, FVector::ZeroVector);
	}
}

// <문성훈> 플레이어측 일반 상태 초기화 InitState 함수 콜하는 노티파이 
void UPlayerAnimInstance::AnimNotify_InitState() {
	if(Player)
		Player->InitState();
}

// <문성훈> 플레이어측 무적 상태 초기화 InitInvincible 함수 콜하는 노티파이
void UPlayerAnimInstance::AnimNotify_EndInvincible() {
	if (Player)
		Player->InitInvincibility();
}

// <문성훈> 플레이어측 상태 전부 초기화하는 함수, 델리게이트에 바인드해서 사용
void UPlayerAnimInstance::InitPlayerState(UAnimMontage* Montage, bool bInterrupted) {
	if (Player) {
		Player->InitState();
		Player->InitInvincibility();
	}
}

// <문성훈> 플레이어 위치로부터 광역 넉백데미지 뿌리는 노티파이
void UPlayerAnimInstance::AnimNotify_AoEKnockBackDamage() {
	if (Player) {
		Player->SpreadAoEDamage(UKnockBackDamageType::StaticClass());
	}
}

// <문성훈> 플레이어 위치로부터 광역 번개데미지 뿌리는 노티파이
void UPlayerAnimInstance::AnimNotify_AoELightningDamage() {
	if (Player) {
		Player->SpreadAoEDamage(ULightningDamageType::StaticClass());
	}
}

// <문성훈> 플레이어 스킬 사용시 각각의 액터 시퀀스 재생
void UPlayerAnimInstance::AnimNotify_PlaySequence() {
	if (Player) {
		// 현재 스킬 번호에 따라 다른 시퀀스 재생
		switch (Player->CurSkill) {
			case 0:
			Player->PlayJudgementCutSequence();
			break;

			case 1:
			Player->PlayEarthquakeSequence();
			break;

			case 2:
			Player->PlayMissileSequence();
			break;
		}
	}
}

// <문성훈> 뇌반 액터 시퀀스 재생 노티파이
void UPlayerAnimInstance::AnimNotify_PlayLightningSequence() {
	if (Player) {
		Player->PlayLightningShockSequence();
	}
}

// <문성훈> 밟고 있는 마테리얼별 다른 발소리 재생 노티파이
void UPlayerAnimInstance::AnimNotify_PlayFootstepSound() {
	if (Player) {
		TArray<AActor*> actorsToIgnore;
		FHitResult outHit;
		actorsToIgnore.Add(Player);
		// 아래쪽으로 라인트레이스 날려서
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Player->GetActorLocation(), Player->GetActorLocation() + FVector(0.f, 0.f, -150.f), ETraceTypeQuery::TraceTypeQuery1, false, actorsToIgnore, EDrawDebugTrace::None, outHit, true)) {
			// 표면 마테리얼 찾아옴
			EPhysicalSurface surfaceType = UGameplayStatics::GetSurfaceType(outHit);
			// 젖은 월드 마테리얼이면
			if (surfaceType == EPhysicalSurface::SurfaceType1) {
				Player->PlayWetFootstepSound(outHit.Location);
			}
			// 마른 월드 마테리얼이면
			else {
				Player->PlayDryFootstepSound(outHit.Location);
			}
		}
	}
}

// <문성훈> 가드 초기화
void UPlayerAnimInstance::AnimNotify_InitGuard() {
	if (Player)
		Player->InitGuard();
}