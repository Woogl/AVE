// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySwordman.h"
#include "AIManager.h"
#include "CombatComponent.h"
#include <Components/CapsuleComponent.h>
#include <AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "AllAVEDamageTypes.h"
#include "AC_Swordman.h"
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetSystemLibrary.h>	
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Components/WidgetComponent.h"
#include "EnemyWidget.h"
#include <Perception/AISense_Damage.h>

// Sets default values
AEnemySwordman::AEnemySwordman()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAC_Swordman::StaticClass();

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempBody(TEXT("SkeletalMesh'/Game/Team/TH/Resources/SciFi_Robot/MESHES/SCIFI_ROBOT_IK_SK.SCIFI_ROBOT_IK_SK'"));
	if (tempBody.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempBody.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		
		static ConstructorHelpers::FObjectFinder<UAnimBlueprint> tempAnim(TEXT("AnimBlueprint'/Game/Team/TH/Animation/ABP_Swordman.ABP_Swordman'"));
		this->GetMesh()->SetAnimInstanceClass(tempAnim.Object->GetAnimBlueprintGeneratedClass());
	}

	Katana = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	Katana->SetupAttachment(GetMesh(), TEXT("SwordSocket"));
	Katana->SetRelativeLocationAndRotation(FVector(-11, -23, -18), FRotator(31, 87.5f, -84));
	Katana->SetCollisionProfileName(TEXT("NoCollision"));
	
	/*enemyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	enemyWidget->SetupAttachment(GetMesh());

	enemyWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	enemyWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> tempWidget(TEXT("WidgetBlueprint'/Game/Team/YEJ/GunEnemy/UI/UI_GunEnemyHP.UI_GunEnemyHP_C'"));
	if (tempWidget.Succeeded())
	{
		enemyWidget->SetWidgetClass(tempWidget.Class);
		enemyWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}*/
}

// Called when the game starts or when spawned
void AEnemySwordman::BeginPlay()
{
	Super::BeginPlay();
	CombatComp->SetupWeapon(Katana);
}

// Called every frame
void AEnemySwordman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemySwordman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemySwordman::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	onGetSet();
	blackboard->SetValueAsEnum(TEXT("AIState"), 4);
	blackboard->SetValueAsObject(TEXT("PlayerActor"), DamageCauser);
	UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, DamageAmount, this->GetActorLocation(), this->GetActorLocation(), NAME_None);
	myManager->StartAI();

	if (blackboard->GetValueAsBool(TEXT("Guard")))
		posture-=DamageAmount;
	hp-=DamageAmount;

	if(hp<=0)
		onDie();
	else if(posture<=0)
		onHitCrushed();


	return 0.0f;
}

void AEnemySwordman::onActionAttack()
{
}

void AEnemySwordman::onHit()
{
}

void AEnemySwordman::onActionGuard()
{
} 

void AEnemySwordman::onHitCrushed()
{
}

void AEnemySwordman::onDie()
{
}

void AEnemySwordman::onGetSet()
{
	Super::onGetSet();
	Katana->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("hand_rSocket"));
	Katana->SetRelativeLocationAndRotation(FVector(-25, -12, 34), FRotator(-66.8f, 62, -48));
}