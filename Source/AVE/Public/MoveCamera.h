// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AVEEnums.h"
#include "MoveCamera.generated.h"

/**
 * ���� �ð� ���� ī�޶� ������ ��ġ�� �̵���Ŵ
 */
UCLASS()
class AVE_API UMoveCamera : public UAnimNotifyState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Custom")
	ECameraPosition NewPosition;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
