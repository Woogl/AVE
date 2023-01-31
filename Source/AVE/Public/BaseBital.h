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
		float hp;			//체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hpMax;		//최대체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float damage;		//공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float posture;		//체간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureMax;	//최대체간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureRecv;	//체간 회복치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float postureTime; //체간 회복 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle tPostureTimer;

	bitalInfo myInfo;

	bitalInfo onHit(int characterDamage);
	void postureRecover();
};
