// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Guard.h"
#include "PlayerCharacter.h"
#include "AIManager.h"
#include <Kismet/GameplayStatics.h>
#include <AIModule/Classes/Perception/AIPerceptionComponent.h>
#include <AIModule/Classes/Perception/AISenseConfig_Damage.h>
#include <AIModule/Classes/BehaviorTree/BehaviorTree.h>

AAC_Guard::AAC_Guard()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Team/TH/Tree/Guard/BT_Guard.BT_Guard'"));
	if (BTObject.Succeeded())
	{
		btGuardTree = BTObject.Object;
	}
	SetPerceptionSystem();
}

void AAC_Guard::BeginPlay()
{
	Super::BeginPlay();
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(),
		AAIManager::StaticClass());
	mEnemyManager = Cast<AAIManager>(FoundActor);
}

void AAC_Guard::OnPossess(APawn* InPawn)
{
	Super::BeginPlay();
	this->RunBehaviorTree(btGuardTree);
}

UBlackboardComponent* AAC_Guard::get_blackboard() const
{
	return blackboard;
}

void AAC_Guard::SetPerceptionSystem()
{
	DamageConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Damage>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));

	GetPerceptionComponent()->SetDominantSense(*DamageConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAC_Guard::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*DamageConfig);
}

void AAC_Guard::OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if (auto const player = Cast<APlayerCharacter>(actor))
	{
		get_blackboard()->SetValueAsEnum(TEXT("AIState"),4);
		get_blackboard()->SetValueAsObject(TEXT("AIState"), player);
	}
}