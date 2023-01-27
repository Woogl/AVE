// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * ABP_PlayerCharacter 만들 때 사용
 */

UCLASS()
class AVE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsBlocking;

	UFUNCTION()
	void AnimNotify_InitState();

	UFUNCTION()
	void AnimNotify_EndInvincible();
};
