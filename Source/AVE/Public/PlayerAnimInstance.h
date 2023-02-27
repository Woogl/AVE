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

	// Foot IK
	void FootIK(float DeltaTime);
	TTuple<bool, float> CapsuleDistance(FName SocketName, ACharacter* Char);
	TTuple<bool, float, FVector> FootLineTrace(FName SocketName, ACharacter* Char);
	UPROPERTY()
	TArray<AActor*> IgnoreActors;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float Displacement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float IKInterpSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator RRot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator LRot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float RIK;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	float LIK;

	// 회피 몽타주
	UFUNCTION()
	void AnimNotify_InitState();

	UFUNCTION()
	void AnimNotify_EndInvincible();

	UFUNCTION()
	void InitPlayerState(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void AnimNotify_AoEKnockBackDamage();

	UFUNCTION()
	void AnimNotify_AoELightningDamage();

	UFUNCTION()
	void AnimNotify_PlaySequence();

	UFUNCTION()
	void AnimNotify_PlayLightningSequence();

	UFUNCTION()
	void AnimNotify_PlayFootstepSound();
};
