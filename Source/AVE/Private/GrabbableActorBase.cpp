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

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("WorldDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	RootComponent = Mesh;
}

void AGrabbableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	// ��������Ʈ ���ε�
	Mesh->OnComponentHit.AddDynamic(this, &AGrabbableActorBase::OnMeshHit);
}

void AGrabbableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* AGrabbableActorBase::GetMesh()
{
	return Mesh;
}

void AGrabbableActorBase::OnGrabbed(ACharacter* InGrabber)
{
	// �ֿ� ��� ���� ����
	Grabber = InGrabber;
	GrabberMesh = Grabber->GetMesh();

	// �ݸ��� ����
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// ������Ʈ �̵���Ű��
	FLatentActionInfo info;
	info.CallbackTarget = this;
	FVector targetLoc = GrabberMesh->GetSocketLocation(TEXT("hand_l"));
	FRotator targetRot = GrabberMesh->GetSocketRotation(TEXT("hand_l"));
	float overTime = 0.4f;
	UKismetSystemLibrary::MoveComponentTo(Mesh, targetLoc, targetRot, true, true, overTime, true, EMoveComponentAction::Move, info);

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
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);

	// �߻� �ӵ� ���
	FVector OutLaunchVelocity;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, GetActorLocation(), ThrowingLoc, 0.f, 0.8f);
	auto temp = Mesh->GetComponentLocation();

	// �߻��ϱ�
	Mesh->AddImpulse(OutLaunchVelocity, NAME_None, true);
	ShouldAttack = true;

	// �ݸ��� ����
	GetWorldTimerManager().SetTimer(ThrowTimer, this, &AGrabbableActorBase::ResetCollisionChannel, 0.15f, false);
}

void AGrabbableActorBase::ResetCollisionChannel()
{
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void AGrabbableActorBase::OnDiscard()
{
	// ������Ʈ ����
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);
}

void AGrabbableActorBase::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
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
	else if (OtherComponent->GetCollisionObjectType() == ECC_WorldStatic && Mesh->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Ignore)
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		ShouldAttack = false;
	}
}

