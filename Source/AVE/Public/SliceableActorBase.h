// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SliceableActorBase.generated.h"

/*
* 잘릴 수 있는 액터를 만들 때 상속받아 사용
*/

UCLASS()
class AVE_API ASliceableActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASliceableActorBase();

	// 잘릴 때 생성되는 메시(복사본)의 원본으로만 쓰이는 메시. (Allow CPU Access = true)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* OriginalMesh;

	// 실제 게임에서 사용할 복사본
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProceduralMeshComponent* ProceduralMesh;

	// (선택사항) 단면에 덮어씌울 머터리얼
	UPROPERTY(EditAnywhere, Category = "Option")
	UMaterialInterface* SectionMaterial = nullptr;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
