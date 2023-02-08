// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AC_Manager.generated.h"

UCLASS()
class AVE_API AAC_Manager : public AAIController
{
	GENERATED_BODY()
	
public:
	AAC_Manager();
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* BT_Manager;
	UPROPERTY(EditAnywhere)
	class UBlackboardData* BB_Manager;
};
