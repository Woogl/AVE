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
	UFUNCTION()
	void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 메시 자르기
	void SliceMesh(UPrimitiveComponent* TargetMesh);

	// 단면에 씌울 머터리얼
	UMaterialInterface* MatForSlicedSection = nullptr;
};
