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

};
