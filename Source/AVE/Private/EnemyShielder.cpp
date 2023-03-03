// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyShielder.h"
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
#include <Blueprint/AIBlueprintHelperLibrary.h>

// Sets default values
AEnemyShielder::AEnemyShielder()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	hp = hpMax = 200;
	posture = postureMax = 100;
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

	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetupAttachment(GetMesh(), TEXT("ShieldSocket"));
	Shield->SetRelativeLocationAndRotation(FVector(-125,-20, -20), FRotator(4,92,185));
	Shield->SetCollisionProfileName(TEXT("NoCollision"));

	enemyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BitalWidget"));
	enemyWidget->SetupAttachment(GetMesh());
	enemyWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	enemyWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UEnemyWidget> tempWidget(TEXT("WidgetBlueprint'/Game/Team/YEJ/GunEnemy/UI/UI_GunEnemyHP.UI_GunEnemyHP_C'"));
	if (tempWidget.Succeeded())
	{
		enemyWidget->SetWidgetClass(tempWidget.Class);
		enemyWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}
}

// Called when the game starts or when spawned
void AEnemyShielder::BeginPlay()
{
	Super::BeginPlay();
	CombatComp->SetupWeapon(Shield);

	UEnemyWidget* enemyBar = Cast<UEnemyWidget>(enemyWidget->GetUserWidgetObject());
	enemyBar->SetOwnerEnemy(this);

	blackboard->SetValueAsEnum(TEXT("AIState"), 0);

}

// Called every frame
void AEnemyShielder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyShielder::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//void  AEnemyShielder::UpdateHealthBar(float currentValue, flaot MaxValue)
//{
//	UEnemyWidget* HealthUI = -Cast<UEnemyWidget>(enemyWidget->GetWidget());
//	if (HealthUI)
//	{
//		if (HealthUI->enemyWidget)
//		{
//			const float Percentage = FMath::Clamp(currentValue / MaxValue, 0.f, 1.f);
//			HealthUI->enemyWidget->Refresh(Percentage);
//		}
//	}
//}

//void AEnemyShielder::ReceivePointDamage(float Damage, const UDamageType* DamageType, FVector HitLocation, FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo)
//{
//	blackboard->SetValueAsObject(TEXT("PlayerActor"), DamageCauser);
//
//	UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage, this->GetActorLocation(), this->GetActorLocation(), NAME_None);
//	myManager->RunAI();
//
//	if (blackboard->GetValueAsBool(TEXT("Guard")))
//		posture -= Damage;
//	hp -= Damage;
//
//	if (hp <= 0)
//		onDie();
//	else if (posture <= 0)
//		onHitCrushed();
//	//else
//	//	onHit();
//}

float AEnemyShielder::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GetWorldTimerManager().ClearTimer(regenTimerHandle);
	GetWorldTimerManager().SetTimer(regenTimerHandle, this, &AEnemyShielder::regenPosture, postureRate, true, postureCool);

	blackboard->SetValueAsObject(TEXT("PlayerActor"), DamageCauser);

	UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, DamageAmount, this->GetActorLocation(), this->GetActorLocation(), NAME_None);
	myManager->RunAI();

	if (blackboard->GetValueAsBool(TEXT("Guard")))
		posture -= DamageAmount;
	hp -= DamageAmount;

	if (hp <= 0)
		onDie();
	else if (posture <= 0)
		onHitCrushed();
	//else
	//	onHit();

	return 0.0f;
}

void AEnemyShielder::onActionAttack()
{
	PlayAnimMontage(enemyAttackMontage, 1, FName("ShielderAttack"));
	blackboard->SetValueAsBool(TEXT("Guard"),false);
	blackboard->SetValueAsEnum(TEXT("AIState"), 6);
}

void AEnemyShielder::onActionGuard()
{
	PlayAnimMontage(enemyAttackMontage, 1, FName("GuardHit0"));
	blackboard->SetValueAsBool(TEXT("Guard"), true);
	blackboard->SetValueAsEnum(TEXT("AIState"), 6);
}

void AEnemyShielder::onHit()
{
	PlayAnimMontage(enemyHitMontage, 1, FName("ShieldHit0"));
	onGetSet();

	blackboard->SetValueAsEnum(TEXT("AIState"), 6);

}

void AEnemyShielder::onHitCrushed()
{
	PlayAnimMontage(enemyHitMontage, 1, FName("Break"));
	blackboard->SetValueAsEnum(TEXT("AIState"), 6);
	this->Tags.Add(FName("Broken"));
}

void AEnemyShielder::onDie()
{
	UAIBlueprintHelperLibrary::GetAIController(this)->K2_ClearFocus();
	PlayAnimMontage(enemyHitMontage, 1, FName("Death0"));
	blackboard->SetValueAsEnum(TEXT("AIState"), 3);
	myManager->EnemyDelete(this);
	Shield->SetCollisionProfileName(FName("Ragdoll"));
	Shield->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Shield->SetSimulatePhysics(true);
	Shield->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//enemyWidget->DestroyComponent();
	enemyWidget->SetVisibility(false);
	this->ActivateRagdoll();
	/*GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);*/
	//this->GetController()->UnPossess();
}

void AEnemyShielder::onGetSet()
{
	Super::onGetSet();
	blackboard->SetValueAsObject(TEXT("PlayerActor"),myManager->PlayerCharacter);
	/*Shield->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("hand_rSocket"));
	Shield->SetRelativeLocationAndRotation(FVector(-25, -12, 34), FRotator(-66.8f, 62, -48));*/
}