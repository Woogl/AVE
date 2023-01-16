// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCamera.h"
#include "PlayerCharacter.h"

void UMoveCamera::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	auto player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (player)
	{
		player->MoveCamera(NewPosition);
	}
}

void UMoveCamera::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	auto player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if (player)
	{
		player->MoveCamera(ECameraPosition::ECP_Default);
	}
}
