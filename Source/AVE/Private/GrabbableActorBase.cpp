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
	
	// ��������Ʈ ���ε�
	Box->OnComponentHit.AddDynamic(this, &AGrabbableActorBase::OnBoxHit);
}

void AGrabbableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrabbableActorBase::OnGrabbed(ACharacter* InGrabber)
{
	// �ֿ� ��� ���� ����
	Grabber = InGrabber;
	GrabberMesh = Grabber->GetMesh();

	// �ݸ��� ����
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// ������Ʈ �̵���Ű��
	FLatentActionInfo info;
	info.CallbackTarget = this;
	FVector targetLoc = GrabberMesh->GetSocketLocation(TEXT("hand_l"));
	FRotator targetRot = GrabberMesh->GetSocketRotation(TEXT("hand_l"));
	float overTime = 0.4f;
	UKismetSystemLibrary::MoveComponentTo(Box, targetLoc, targetRot, true, true, overTime, true, EMoveComponentAction::Move, info);

	// ������Ʈ ���̱�
	GetWorldTimerManager().SetTimer(AttachTimer, this, &AGrabbableActorBase::AttachToGrabber, overTime, false);
}

void AGrabbableActorBase::AttachToGrabber()
{
	AttachToComponent(GrabberMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), TEXT("hand_l"));
}

void AGrabbableActorBase::OnThrown(FVector ThrowingLoc)
{
	// ������Ʈ ����
	Box->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Box->SetSimulatePhysics(true);

	// �߻� �ӵ� ���
	FVector OutLaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, GetActorLocation(), ThrowingLoc, 0.f, 0.8f);
	auto temp = Box->GetComponentLocation();

	// �߻��ϱ�
	Box->AddImpulse(OutLaunchVelocity, NAME_None, true);
	ShouldAttack = true;

	// �������� 0.05�� �� �ݸ��� ����
	GetWorldTimerManager().SetTimer(ThrowTimer, this, &AGrabbableActorBase::ResetCollisionChannel, 0.05f, false);
}

void AGrabbableActorBase::ResetCollisionChannel()
{
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AGrabbableActorBase::OnDiscard()
{
	// ������Ʈ ����
	Box->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Box->SetSimulatePhysics(true);
}

void AGrabbableActorBase::OnBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���� üũ
	if (ShouldAttack == false) return;

	// ĳ�������� üũ
	if (OtherActor->IsA(ACharacter::StaticClass()) && OtherActor != Grabber)
	{
		FHitResult outHit;
		outHit.Item = (int32)DamageType;
		UGameplayStatics::ApplyPointDamage(OtherActor, BaseDamage, GetActorLocation(), outHit, Grabber->GetInstigatorController(), this, UDamageType::StaticClass());

		// Ÿ�� VFX
		if (HitNiagara)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, GetActorLocation(), GetActorRotation());
		}
		// Ÿ�� SFX
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

