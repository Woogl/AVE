// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSlicer.generated.h"

UCLASS()
class AVE_API AMeshSlicer : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;
	
public:	
	AMeshSlicer();

protected:
	virtual void BeginPlay() override;

public:	
	//UFUNCTION()
	//void OnBoxOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// �޽� �ڸ���
	void SliceMesh(UPrimitiveComponent* TargetMesh);

	// �ܸ鿡 ���� ���͸���
	UMaterial* MatForSlicedSection = nullptr;
};
