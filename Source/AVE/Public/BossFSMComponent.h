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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	void IdleFSM();

	FTimerHandle idleTimer;
};
