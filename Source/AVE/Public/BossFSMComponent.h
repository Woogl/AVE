// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSMComponent.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle UMETA(Displayname = IDLE),
	Walk UMETA(Displayname = WALK),
	Move UMETA(Displayname = MOVE),
	DashATK UMETA(Displayname = DashATK),
	NormalATK UMETA(Displayname = NormalATK),
	ATK01 UMETA(Displayname = ATK01),
	ATK02 UMETA(Displayname = ATK02),
	ATK03 UMETA(Displayname = ATK03),
	JumpATK UMETA(Displayname = JumpATK),
	Backstep UMETA(Displayname = Backstep),
	BehindATK UMETA(Displayname = BehindATK),
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class AVE_API UBossFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBossFSMComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossState bossStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABoss* asBoss;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBossAnimInstance* asBossAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerCharacter* asPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AAIController* AiCtrl;
	
	void TickIdle();
	void TickWalk();
	void TickMove();
	void TickDashAtk();
	void TickNormalAtk();
	void TickATK01();
	void TickATK02();
	void TickATK03();
	void TickJumpATK();
	void TickBackstep();
	void TickBehindATK();

	UFUNCTION(BlueprintImplementableEvent)
	void IdleDelay();

	UFUNCTION(BlueprintImplementableEvent)
	void WalkDelay();

	UFUNCTION(BlueprintImplementableEvent)
	void SetWalkRandomInt();
	
	UFUNCTION(BlueprintImplementableEvent)
	void WalkToLocation(float locRange);

	UFUNCTION(BlueprintImplementableEvent)
	void MoveToLocation();

	UFUNCTION()
	void MoveToFSM();
	
	UFUNCTION()
	void SetMoveSpeed();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int walkRandomInt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int seqValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle WriTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float randomPercent;

	float dashATKPercent = 0.2;
};
