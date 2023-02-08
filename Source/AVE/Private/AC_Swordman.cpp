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
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Team/TH/Tree/Swordman/BB_Swordman.BB_Swordman'"));
	if (BBObject.Succeeded())
	{
		BB_Swordman = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Team/TH/Tree/Swordman/BT_Swordman.BT_Swordman'"));
	if (BBObject.Succeeded())
	{
		BT_Swordman = BTObject.Object;
	}

	SetPerceptionSystem();
}

void AAC_Swordman::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BT_Swordman);
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
	//if (auto const player = Cast<APlayerCharacter>(actor))
	//{
	//	get_blackboard()->SetValueAsEnum(TEXT("AIState"),4);
	//	get_blackboard()->SetValueAsObject(TEXT("PlayerActor"), player);
	//}
}