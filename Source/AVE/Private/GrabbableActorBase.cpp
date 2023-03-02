// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableActorBase.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include "AllAVEDamageTypes.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Components/SphereComponent.h>

AGrabbableActorBase::AGrabbableActorBase()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("WorldDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;

	ElectricArc = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricArc"));
	ElectricArc->SetupAttachment(RootComponent);
	ElectricArc->bAutoActivate = false;

	HighlightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HighlightSphere"));
	HighlightSphere->SetupAttachment(RootComponent);
	HighlightSphere->SetSphereRadius(150.f);
	HighlightSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	HighlightSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECR_Overlap);

	// ����� Ÿ�� �ʱⰪ
	DamageType = UStandardDamageType::StaticClass();

	// ��������Ʈ ���ε�
	Mesh->OnComponentHit.AddDynamic(this, &AGrabbableActorBase::OnMeshHit);
	HighlightSphere->OnComponentBeginOverlap.AddDynamic(this, &AGrabbableActorBase::OnHighlightSphereOverlapBegin);
	HighlightSphere->OnComponentEndOverlap.AddDynamic(this, &AGrabbableActorBase::OnHighlightSphereOverlapEnd);
}

void AGrabbableActorBase::BeginPlay()
{
	Super::BeginPlay();

}

UStaticMeshComponent* AGrabbableActorBase::GetMesh() const
{
	return Mesh;
}

void AGrabbableActorBase::OnGrabbed()
{
	if (PlayerMesh == nullptr)
	{
		PlayerMesh = UGameplayStatics::GetPlayerCharacter(this, 0)->GetMesh();
	}

	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	Mesh->SetSimulatePhysics(false);

	GetWorldTimerManager().SetTimer(ElectricArcTimer, this, &AGrabbableActorBase::StartElectricArc, 0.016f, true, 0.2f);

	// ������ �����Լ�
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			ElectricArc->Activate(true);
		}), 0.2f, false);
}

void AGrabbableActorBase::StartElectricArc()
{
	ElectricArc->SetVectorParameter(TEXT("BeamEnd"), PlayerMesh->GetSocketLocation("middle_03_l"));
}

void AGrabbableActorBase::EndElectricArc()
{
	ElectricArc->Deactivate();

	GetWorldTimerManager().ClearTimer(ElectricArcTimer);
}

void AGrabbableActorBase::OnPushed()
{
	EndElectricArc();

	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);

	bShouldAttack = true;
}

void AGrabbableActorBase::OnDropped()
{
	EndElectricArc();

	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	Mesh->SetSimulatePhysics(true);
}

void AGrabbableActorBase::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���� üũ
	if (bShouldAttack == false) return;

	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// ĳ���͸� ������ ����
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		FHitResult outHit;
		UGameplayStatics::ApplyPointDamage(OtherActor, BaseDamage, GetActorLocation(), outHit, nullptr, this, DamageType);
	}

	// �ı� ȿ��
	FractureMesh();

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

	bShouldAttack = false;
}

void AGrabbableActorBase::OnHighlightSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Mesh->SetRenderCustomDepth(true);
}

void AGrabbableActorBase::OnHighlightSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Mesh->SetRenderCustomDepth(false);
}

