// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AVE_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBossAnimInstance();
	virtual void NativeBeginPlay() override;
	// virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABoss* asBoss;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APawn* playerPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerCharacter* asPlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector velocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFalling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int activeChildIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int lightningVelocityZ = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int lightningVelocityMZ = -3000;
	
	UFUNCTION()
	void AnimNotify_WarCryBegin();
	UFUNCTION()
	void AnimNotify_WarCryEnd();
	UFUNCTION()
	void WarCryRadialATK();

	UFUNCTION()
	void AnimNotify_OnSuperArmor();
	UFUNCTION()
	void AnimNotify_OffSuperArmor();

	UFUNCTION()
	void AnimNotify_LightningBegin();
	UFUNCTION()
	void AnimNotify_LightningEnd();
	UFUNCTION()
	void AnimNotify_LightningRadialATK();

	UFUNCTION()
	void AnimNotify_FlyingTrue();
	UFUNCTION()
	void AnimNotify_FlyingFalse();

	UFUNCTION()
	void AnimNotify_BladeRangeBegin();
	UFUNCTION()
	void AnimNotify_BladeRangeEnd();
	UFUNCTION()
	void AnimNotify_BladeRangeRadialATK();

	UFUNCTION()
	void AnimNotify_PatternATK();
	UFUNCTION()
	void AnimNotify_ReturnToMove();
	
	FTimerHandle warCryTimerHandle;
	FTimerHandle ligntningTimerHandle;
	FTimerHandle bladeRangeTimerHandle;
	
	bool bTakenWarCryDamage = false;
	bool bDoOnce = false;
	bool bParry = false;
};