// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SliceableActorBase.generated.h"

/*
* �߸� �� �ִ� ���͸� ���� �� ��ӹ޾� ���
*/

UCLASS()
class AVE_API ASliceableActorBase : public AActor
{
	GENERATED_BODY()

public:
	ASliceableActorBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// �߸� �� �����Ǵ� �޽�(���纻)�� �������θ� ���̴� �޽�. (Allow CPU Access = true)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* OriginalMesh;

	// ���� ���ӿ��� ����� ���纻
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	class USoundBase* SliceSound;

	// (���û���) �ܸ鿡 ����� ���͸���
	UPROPERTY(EditAnywhere, Category = "Option")
	UMaterialInterface* SectionMaterial = nullptr;

protected:
	virtual void BeginPlay() override;
};
