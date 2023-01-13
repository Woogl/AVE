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

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	ACharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance", meta = (AllowPrivateAccess = "true"))
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance", meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Instance", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

public:
	void SetIsBlocking(bool Value);
};
