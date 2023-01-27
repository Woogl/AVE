// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "DetourCrowdAIController.h"
//#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
//#include <AIModule/Classes/Perception/AIPerceptionTypes.h>
//#include "AC_Guard.generated.h"
//
//UCLASS()
//class AVE_API AAC_Guard : public AAIController
//{
//	GENERATED_BODY()
//	AAC_Guard();
//public:
//	void BeginPlay() override;
//	void OnPossess(APawn* InPawn) override;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class AAIManager* mEnemyManager;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class UBehaviorTree* btGuardTree;
//
//	class UBlackboardComponent* get_blackboard() const;
//	class UBlackboardComponent* blackboard;
//
//	class UAISenseConfig_Damage* DamageConfig;
//
//	UFUNCTION()
//		void OnTargetDetected(AActor* actor, FAIStimulus const Stimulus);
//	UFUNCTION()
//		void SetPerceptionSystem();
//};