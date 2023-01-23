// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableActorBase.h"
#include <Components/BoxComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include "Dummy.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>

// Sets default values
AGrabbableActorBase::AGrabbableActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Box->SetSimulatePhysics(true);
	Box->BodyInstance.bNotifyRigidBodyCollision = true;
	RootComponent = Box;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	Mesh->SetupAttachment(RootComponent);
}

void AGrabbableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 델리게이트 바인딩
	Box->OnComponentHit.AddDynamic(this, &AGrabbableActorBase::OnBoxHit);
}

void AGrabbableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrabbableActorBase::OnGrabbed(ACharacter* InGrabber)
{
	// 주운 사람 정보 설정
	Grabber = InGrabber;
	GrabberMesh = Grabber->GetMesh();

	// 콜리전 변경
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// 컴포넌트 이동시키기
	FLatentActionInfo info;
	info.CallbackTarget = this;
	FVector targetLoc = GrabberMesh->GetSocketLocation(TEXT("hand_l"));
	FRotator targetRot = GrabberMesh->GetSocketRotation(TEXT("hand_l"));
	float overTime = 0.4f;
	UKismetSystemLibrary::MoveComponentTo(Box, targetLoc, targetRot, true, true, overTime, true, EMoveComponentAction::Move, info);

	// 컴포넌트 붙이기
	GetWorldTimerManager().SetTimer(AttachTimer, this, &AGrabbableActorBase::AttachToGrabber, overTime, false);
}

void AGrabbableActorBase::AttachToGrabber()
{
	AttachToComponent(GrabberMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), TEXT("hand_l"));
}

void AGrabbableActorBase::OnThrown(FVector ThrowingLoc)
{
	// 컴포넌트 떼기
	Box->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Box->SetSimulatePhysics(true);

	// 발사 속도 계산
	FVector OutLaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, GetActorLocation(), ThrowingLoc, 0.f, 0.8f);
	auto temp = Box->GetComponentLocation();

	// 발사하기
	Box->AddImpulse(OutLaunchVelocity, NAME_None, true);
	ShouldAttack = true;

	// 던져지고 0.05초 후 콜리전 변경
	GetWorldTimerManager().SetTimer(ThrowTimer, this, &AGrabbableActorBase::ResetCollisionChannel, 0.05f, false);
}

void AGrabbableActorBase::ResetCollisionChannel()
{
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AGrabbableActorBase::OnDiscard()
{
	// 컴포넌트 떼기
	Box->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Box->SetSimulatePhysics(true);
}

void AGrabbableActorBase::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// 조건 체크
	if (ShouldAttack == false) return;

	// 캐릭터인지 체크
	if (OtherActor->IsA(ACharacter::StaticClass()) && OtherActor != Grabber)
	{
		FHitResult outHit;
		outHit.Item = (int32)DamageType;
		UGameplayStatics::ApplyPointDamage(OtherActor, BaseDamage, GetActorLocation(), outHit, Grabber->GetInstigatorController(), this, UDamageType::StaticClass());

		// 타격 VFX
		if (HitNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, GetActorLocation(), GetActorRotation());
		}
		// 타격 SFX
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
	}
	else if (OtherComponent->GetCollisionObjectType() == ECC_WorldStatic)
	{
		ShouldAttack = false;
	}
}

