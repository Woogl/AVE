// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Swordman.h"
#include "PlayerCharacter.h"
#include "AIManager.h"
#include <Kismet/GameplayStatics.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <AIModule/Classes/Perception/AIPerceptionComponent.h>
#include <AIModule/Classes/Perception/AISenseConfig_Damage.h>
#include <AIModule/Classes/BehaviorTree/BehaviorTree.h>

AAC_Swordman::AAC_Swordman()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Team/TH/Tree/Shielder/BB_Shielder.BB_Shielder'"));
	if (BBObject.Succeeded())
	{
		BB_Shielder = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Team/TH/Tree/Shielder/BT_Shielder.BT_Shielder'"));
	if (BBObject.Succeeded())
	{
		BT_Shielder = BTObject.Object;
	}

	SetPerceptionSystem();
}

void AAC_Swordman::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BT_Shielder);
}

UBlackboardComponent* AAC_Swordman::get_blackboard() const
{
	return blackboard;
}

void AAC_Swordman::SetPerceptionSystem()
{
	DamageConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	GetPerceptionComponent()->SetDominantSense(*DamageConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAC_Swordman::OnTargetPerceptionUpdated);
	GetPerceptionComponent()->ConfigureSense(*DamageConfig);
}

void AAC_Swordman::OnTargetPerceptionUpdated(AActor* actor, FAIStimulus const Stimulus)
{
	if (auto const player = Cast<APlayerCharacter>(actor))
	{
		this->Blackboard->SetValueAsObject(TEXT("PlayerActor"), player);
		this->Blackboard->SetValueAsEnum(TEXT("AIState"),4);
		//get_blackboard()
	}
}