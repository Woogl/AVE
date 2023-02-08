// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Manager.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAC_Manager::AAC_Manager()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Team/TH/Tree/Manager/BB_Manager.BB_Manager'"));
	if (BBObject.Succeeded())
	{
		BB_Manager = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Team/TH/Tree/Manager/BT_Manager.BT_Manager'"));
	if (BBObject.Succeeded())
	{
		BT_Manager = BTObject.Object;
	}
}

void AAC_Manager::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BT_Manager);
}