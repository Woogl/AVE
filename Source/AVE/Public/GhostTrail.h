// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/TimelineComponent.h>
#include "GhostTrail.generated.h"

UCLASS()
class AVE_API AGhostTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	AGhostTrail();

	/** Ghost trail material  */
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInterface* GhostTrailMaterial;

	/** Ghost trail fade opacity parameter name  */
	UPROPERTY(EditDefaultsOnly, Category = "Opacity")
	FName ScalarParameterName = FName(TEXT("Opacity"));

	/** Ghost trail mesh opacity curve  */
	UPROPERTY(EditDefaultsOnly, Category = "Opacity")
	UCurveFloat* OpacityCurve;

	/** Ghost trail pose character  */
	UPROPERTY(BlueprintReadWrite, Category = "Character", meta = (ExposeOnSpawn = "true"))
	ACharacter* CharacterRef;

protected:
	virtual void BeginPlay() override;

	/** Ghost trail mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UPoseableMeshComponent* PoseableMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	USceneComponent* SceneRootComp;

	/** Ghost trail mesh opacity timeline  */
	UPROPERTY()
	FTimeline OpacityTimeline;

	/** Ghost trail dynamic material  */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMatInstance;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "GhostTrail")
	void BeginGhostTrailEffect();

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UFUNCTION()
	void HandleTimelineFinished();
};
