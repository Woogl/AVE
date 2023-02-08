// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <AIModule/Classes/Perception/AIPerceptionTypes.h>
#include "AC_Swordman.generated.h"

/**
 * 
 */
UCLASS()
class AVE_API AAC_Swordman : public AAIController
{
	GENERATED_BODY()
public:
	AAC_Swordman();
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
		class UBehaviorTree* BT_Swordman;
	UPROPERTY(EditAnywhere)
		class UBlackboardData* BB_Swordman;

	class UAISenseConfig_Damage* DamageConfig;
	class UBlackboardComponent* get_blackboard() const;
	class UBlackboardComponent* blackboard;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* actor, FAIStimulus const Stimulus);
	UFUNCTION()
		void SetPerceptionSystem();
};
