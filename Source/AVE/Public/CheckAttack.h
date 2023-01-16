// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AVEEnums.h"
#include "CheckAttack.generated.h"

/**
 * 지속 시간 동안 공격 판정 및 대미지 처리 발동
 */
UCLASS()
class AVE_API UCheckAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// 가할 데미지
	UPROPERTY(EditAnywhere, Category = "Custom", meta = (ClampMin = 0.f))
	float BaseDamage = 10.f;

	// 공격 타입 (ex: 경직, 넉백, 다운 등)
	UPROPERTY(EditAnywhere, Category = "Custom")
	EDamageType DamageType;

	// 역경직 시간
	UPROPERTY(EditAnywhere, Category = "Custom", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float HitStopDuration;

	// 자르기 가능하게 할지
	UPROPERTY(EditAnywhere, Category = "Custom")
	bool bShouldSlice = false;

	UPROPERTY()
	class UCombatComponent* CombatComp;	
};
