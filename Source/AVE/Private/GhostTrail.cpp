// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostTrail.h"
#include <Components/PoseableMeshComponent.h>
#include <GameFramework/Character.h>

AGhostTrail::AGhostTrail()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = SceneRootComp;

	PoseableMeshComp = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComp"));
	PoseableMeshComp->SetupAttachment(RootComponent);
}

void AGhostTrail::BeginPlay()
{
	Super::BeginPlay();

	BeginGhostTrailEffect();
}

void AGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OpacityTimeline.TickTimeline(DeltaTime);	// Timeline에 DeltaTime을 적용
}

void AGhostTrail::BeginGhostTrailEffect()
{
	if (OpacityCurve && CharacterRef && GhostTrailMaterial)
	{
		PoseableMeshComp->SetSkeletalMesh(CharacterRef->GetMesh()->SkeletalMesh);
		PoseableMeshComp->CopyPoseFromSkeletalComponent(CharacterRef->GetMesh());

		DynamicMatInstance = UMaterialInstanceDynamic::Create(GhostTrailMaterial, this);
		TArray <FSkeletalMaterial> materials = CharacterRef->GetMesh()->SkeletalMesh->GetMaterials();

		for (int32 Index = 0; Index < materials.Num(); ++Index)
		{
			PoseableMeshComp->SetMaterial(Index, DynamicMatInstance);
		}

		FOnTimelineFloat TimelineUpdateCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		OpacityTimeline.SetLooping(false);

		TimelineUpdateCallback.BindUFunction(this, FName{ TEXT("HandleTimelineUpdate" }));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("HandleTimelineFinished") });

		OpacityTimeline.AddInterpFloat(OpacityCurve, TimelineUpdateCallback);
		OpacityTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		OpacityTimeline.PlayFromStart();
	}
}

void AGhostTrail::HandleTimelineUpdate(float Value)
{
	DynamicMatInstance->SetScalarParameterValue(ScalarParameterName, Value);
}

void AGhostTrail::HandleTimelineFinished()
{
	Destroy();
}
