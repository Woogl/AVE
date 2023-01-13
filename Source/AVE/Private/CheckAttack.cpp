// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckAttack.h"
#include "CombatComponent.h"

void UCheckAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	CombatComp = Cast<UCombatComponent>(MeshComp->GetOwner()->GetComponentByClass(UCombatComponent::StaticClass()));
	if (CombatComp)
	{
		CombatComp->AttackCheckBegin();
		CombatComp->SetDamageInfo(BaseDamage, AttackType);
		CombatComp->HitstopTime = HitStopDuration;
		CombatComp->bEnableSlice = bShouldSlice;
	}
}

void UCheckAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (CombatComp)
	{
		CombatComp->AttackCheckTick();
	}
}

void UCheckAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (CombatComp)
	{
		CombatComp->AttackCheckEnd();
	}
}
