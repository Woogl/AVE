// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableActorBase.h"
#include <Components/BoxComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include "AllAVEDamageTypes.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

AGrabbableActorBase::AGrabbableActorBase()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("WorldDynamic"));
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	RootComponent = Mesh;

	ElectricArc = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricArc"));
	ElectricArc->bAutoActivate = false;
	ElectricArc->SetupAttachment(RootComponent);

	// ����� Ÿ�� �ʱⰪ
	DamageType = UStandardDamageType::StaticClass();
}

void AGrabbableActorBase::BeginPlay()
{
	Super::BeginPlay();

	// ��������Ʈ ���ε�
	Mesh->OnComponentHit.AddDynamic(this, &AGrabbableActorBase::OnMeshHit);
}

UStaticMeshComponent* AGrabbableActorBase::GetMesh() const
{
	return Mesh;
}

void AGrabbableActorBase::OnGrabbed()
{
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
	ElectricArc->SetVectorParameter(TEXT("BeamEnd"), UGameplayStatics::GetPlayerCharacter(this, 0)->GetMesh()->GetSocketLocation(TEXT("hand_r")));
}

void AGrabbableActorBase::EndElectricArc()
{
	ElectricArc->Deactivate();

	GetWorldTimerManager().ClearTimer(ElectricArcTimer);
}

void AGrabbableActorBase::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���� üũ
	if (bShouldAttack == false) return;

	// ĳ�������� üũ
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		FHitResult outHit;
		UGameplayStatics::ApplyPointDamage(OtherActor, BaseDamage, GetActorLocation(), outHit, nullptr, this, DamageType);

		FractureMesh();

		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

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
}

