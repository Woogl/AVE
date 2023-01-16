// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AVEEnums.h"
#include "CheckAttack.generated.h"

/**
 * ���� �ð� ���� ���� ���� �� ����� ó�� �ߵ�
 */
UCLASS()
class AVE_API UCheckAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// ���� ������
	UPROPERTY(EditAnywhere, Category = "Custom", meta = (ClampMin = 0.f))
	float BaseDamage = 10.f;

	// ���� Ÿ�� (ex: ����, �˹�, �ٿ� ��)
	UPROPERTY(EditAnywhere, Category = "Custom")
	EDamageType DamageType;

	// ������ �ð�
	UPROPERTY(EditAnywhere, Category = "Custom", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float HitStopDuration;

	// �ڸ��� �����ϰ� ����
	UPROPERTY(EditAnywhere, Category = "Custom")
	bool bShouldSlice = false;

	UPROPERTY()
	class UCombatComponent* CombatComp;	
};
