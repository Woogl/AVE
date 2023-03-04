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

	// 델리게이트 바인딩
	Box->OnComponentEndOverlap.AddDynamic(this, &AMeshSlicer::OnBoxOverlapEnd);

	// 스폰되고 0.01초 지나면 파괴
	InitialLifeSpan = 0.01f;

	// 디버그용
	//Box->bHiddenInGame = false;
}

void AMeshSlicer::BeginPlay()
{
	Super::BeginPlay();

}

void AMeshSlicer::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 단면에 덮어씌울 머터리얼 가져오기
	ASliceableActorBase* sliceTarget = Cast<ASliceableActorBase>(OtherActor);

	if (sliceTarget)
	{
		MatForSlicedSection = sliceTarget->SectionMaterial;
		
		// 충돌한 메시 자르고 파괴
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
	// 자를 메시
	UProceduralMeshComponent* meshToSlice = Cast<UProceduralMeshComponent>(TargetMesh);
	FVector planePosition = Box->GetComponentLocation();
	FVector planeNormal = Box->GetUpVector();

	// 잘라진 메시
	UProceduralMeshComponent* otherHalfMesh;

	// 메시 자르기
	if (meshToSlice != nullptr)
	{
		if (MatForSlicedSection != nullptr)
		{
			// 지정된 절단면 머터리얼 있으면 그걸 씌워줌
			UKismetProceduralMeshLibrary::SliceProceduralMesh(meshToSlice, planePosition, planeNormal, true, otherHalfMesh, EProcMeshSliceCapOption::CreateNewSectionForCap, MatForSlicedSection);
		}
		else
		{
			// 지정 안했으면 원래 머터리얼을 덮어씌움
			UKismetProceduralMeshLibrary::SliceProceduralMesh(meshToSlice, planePosition, planeNormal, true, otherHalfMesh, EProcMeshSliceCapOption::UseLastSectionForCap, MatForSlicedSection);
		}
		// 잘라진 메시 분리
		otherHalfMesh->SetSimulatePhysics(true);
		otherHalfMesh->AddRadialImpulse(planePosition, 1000.f, 500.f, ERadialImpulseFalloff::RIF_Constant, true);
		otherHalfMesh->SetAngularDamping(2.f);
		meshToSlice->AddRadialImpulse(planePosition, 1000.f, 500.f, ERadialImpulseFalloff::RIF_Constant, true);
	}
}
