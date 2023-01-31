// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseBital.generated.h"

struct bitalInfo {
	float hp;
	float hpMax;
	float posture;
	float postureMax;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AVE_API UBaseBital : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseBital();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void init(float getHp, float getPosture, float getDamage, float recv, float time);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hp;			//ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hpMax;		//�ִ�ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float damage;		//���ݷ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posture;		//ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureMax;	//�ִ�ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureRecv;	//ü�� ȸ��ġ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureTime; //ü�� ȸ�� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle tPostureTimer;

	bitalInfo myInfo;

	bitalInfo onHit(int characterDamage);
	void postureRecover();
};
