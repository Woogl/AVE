// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSlicer.h"
#include <Components/BoxComponent.h>
#include "SliceableActorBase.h"
#include "KismetProceduralMeshLibrary.h"
#include <Kismet/GameplayStatics.h>

AMeshSlicer::AMeshSlicer()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetBoxExtent(FVector(30.f, 50.f, 0.f));
	RootComponent = Box;

	// ��������Ʈ ���ε�
	Box->OnComponentEndOverlap.AddDynamic(this, &AMeshSlicer::OnBoxOverlapEnd);

	// �����ǰ� 0.01�� ������ �ı�
	InitialLifeSpan = 0.01f;

	// ����׿�
	//Box->bHiddenInGame = false;
}

void AMeshSlicer::BeginPlay()
{
	Super::BeginPlay();

}

void AMeshSlicer::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �ܸ鿡 ����� ���͸��� ��������
	ASliceableActorBase* sliceTarget = Cast<ASliceableActorBase>(OtherActor);

	if (sliceTarget)
	{
		MatForSlicedSection = sliceTarget->SectionMaterial;
		
		// �浹�� �޽� �ڸ��� �ı�
		SliceMesh(OtherComp);

		// SFX
		if (USoundBase* sfx = sliceTarget->SliceSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, sfx, GetActorLocation());
		}
	}
}

void AMeshSlicer::SliceMesh(UPrimitiveComponent* TargetMesh)
{
	// �ڸ� �޽�
	UProceduralMeshComponent* meshToSlice = Cast<UProceduralMeshComponent>(TargetMesh);
	FVector planePosition = Box->GetComponentLocation();
	FVector planeNormal = Box->GetUpVector();

	// �߶��� �޽�
	UProceduralMeshComponent* otherHalfMesh;

	// �޽� �ڸ���
	if (meshToSlice != nullptr)
	{
		if (MatForSlicedSection != nullptr)
		{
			// ������ ���ܸ� ���͸��� ������ �װ� ������
			UKismetProceduralMeshLibrary::SliceProceduralMesh(meshToSlice, planePosition, planeNormal, true, otherHalfMesh, EProcMeshSliceCapOption::CreateNewSectionForCap, MatForSlicedSection);
		}
		else
		{
			// ���� �������� ���� ���͸����� �����
			UKismetProceduralMeshLibrary::SliceProceduralMesh(meshToSlice, planePosition, planeNormal, true, otherHalfMesh, EProcMeshSliceCapOption::UseLastSectionForCap, MatForSlicedSection);
		}
		// �߶��� �޽� �и�
		otherHalfMesh->SetSimulatePhysics(true);
		otherHalfMesh->AddRadialImpulse(planePosition, 1000.f, 500.f, ERadialImpulseFalloff::RIF_Constant, true);
		otherHalfMesh->SetAngularDamping(2.f);
		meshToSlice->AddRadialImpulse(planePosition, 1000.f, 500.f, ERadialImpulseFalloff::RIF_Constant, true);
	}
}
