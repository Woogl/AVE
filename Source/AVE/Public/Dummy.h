// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EBodyPart.h"
#include "Dummy.generated.h"

UCLASS()
class AVE_API ADummy : public ACharacter
{
	GENERATED_BODY()

public:
	ADummy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �÷��̾� �Ĵٺ���
	void LookAtPlayer();

	// ������ ���� ���� ��
	UFUNCTION(BlueprintCallable)
	void OnFinishered();

	// ��ü ����
	UFUNCTION(BlueprintCallable)
	void SliceBodyPart(EBodyPart BodyIndex, FVector Impulse, float RagdollDelay);

	// ���׵� Ȱ��ȭ
	UFUNCTION(BlueprintCallable)
	void ActivateRagdoll();

	// ���׵� Ȱ��ȭ Ÿ�̸�
	FTimerHandle RagdollTimer;

	// �����
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Hit Reactions")
	TArray<UAnimMontage*> Hits;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Hit Reactions")
	TArray<UAnimMontage*> Knockbacks;
	UPROPERTY(EditDefaultsOnly, Category = "Montages | Finishered Reactions")
	TArray<UAnimMontage*> Finishered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;
};
