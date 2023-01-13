// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSlicer.h"
#include <Components/BoxComponent.h>
#include "SliceableActorBase.h"
#include "KismetProceduralMeshLibrary.h"

AMeshSlicer::AMeshSlicer()
{
	//PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetCollisionProfileName(TEXT("SliceQuery"));
	// �ڸ� ũ�� ����
	Box->SetBoxExtent(FVector(15.f, 45.f, 0.f));
	RootComponent = Box;

	// ��������Ʈ ���ε�
	//Box->OnComponentBeginOverlap.AddDynamic(this, &AMeshSlicer::OnBoxOverlapBegin);
	Box->OnComponentEndOverlap.AddDynamic(this, &AMeshSlicer::OnBoxOverlapEnd);

	// �����ǰ� 0.01�� ������ �ı�
	InitialLifeSpan = 0.01f;

	// ����׿�
	//Box->bHiddenInGame = false;
}

void AMeshSlicer::BeginPlay()
{
	Super::BeginPlay();
	
	//�����
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SpawnMeshSlicer"));
}

void AMeshSlicer::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �ܸ鿡 ����� ���͸��� ��������
	auto sliceTarget = Cast<ASliceableActorBase>(OtherActor);
	MatForSlicedSection = sliceTarget->SectionMaterial;

	// �浹�� �޽� �ڸ��� �ı�
	SliceMesh(OtherComp);
}

void AMeshSlicer::SliceMesh(UPrimitiveComponent* TargetMesh)
{
	// �����
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SliceMesh"));

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
		meshToSlice->AddRadialImpulse(planePosition, 1000.f, 500.f, ERadialImpulseFalloff::RIF_Constant, true);
	}
}

/*
void AMeshSlicer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
