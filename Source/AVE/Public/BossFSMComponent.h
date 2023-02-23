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
	SlashATK UMETA(Displayname = SlashATK),
	ComboATK UMETA(Displayname = ComboATK),
	JumpATK UMETA(Displayname = JumpATK),
	StanceATK UMETA(Displayname = StanceATK),
	BackStep UMETA(Displayname = BackStep),
	BladeRangeATK UMETA(Displayname = BladeRangeATK),
	BehindATK UMETA(Displayname = BehindATK),
	GrabATK UMETA(Displayname = GrabATK),
	WarCry UMETA(Displayname = WarCry),
	Insal UMETA(Displayname = Insal),
	LaserRangeATK UMETA(Displayname = LaserRangeATK),
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
	void TickDashATK();
	void TickNormalATK();
	void TickATK01();
	void TickSlashATK();
	void TickComboATK();
	void TickJumpATK();
	void TickStanceATK();
	void TickBackStep();
	void TickBladeRangeATK();
	void TickBehindATK();
	void TickGrabATK();
	void TickWarCry();
	void TickInsal();
	void TickLaserRangeATK();

	UFUNCTION(BlueprintImplementableEvent)
	void BossStateDebug();
	
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

	UFUNCTION(BlueprintImplementableEvent)
	void GuardSelect();
	
	UFUNCTION()
	void MoveToFSM();

	UFUNCTION()
	void ReturnToMove();

	UFUNCTION()
	void ReturnToWalk();

	UFUNCTION()
	void ReturnToNormalATK();
	
	UFUNCTION()
	void ReturnToBladeRangeATK();

	UFUNCTION()
	void ReturnToStanceATK();

	UFUNCTION()
	void ReturnToSlashATK();

	UFUNCTION()
	void ReturnToSecondPhase();
	
	UFUNCTION()
	void ReturnToWarCry();

	UFUNCTION()
	void ReturnToLaserRangeATK();
	
	UFUNCTION()
	void SetMoveSpeed();

	UFUNCTION(BlueprintCallable)
	void SetGuardMoveSpeed();
	UFUNCTION(BlueprintCallable)
	void GuardOrBackStep();
	
	UFUNCTION()
	void RandomInt(int min, int max);

	UFUNCTION()
	void RandomFloat();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int walkRandomInt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle WriTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle montageDelayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle delayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle secondPhaseHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float randomFloatValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int randomIntValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasExecuted;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGuarding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSecondPhase;

	bool bDoOnce = false;
	
	float dashATKPercent = 0.3f;
	float backStepPercent = 0.3f;
	float slashATKPercent = 0.3f;
	float normalATKPercent = 0.6f;
	float laserRangeATKPercent = 1.f;
};
