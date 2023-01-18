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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference")
	ACharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance")
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance")
	bool bIsBlocking;

	// 회피 몽타주
	UFUNCTION()
	void AnimNotify_StartiFrame();
	UFUNCTION()
	void AnimNotify_EndiFrame();
};
