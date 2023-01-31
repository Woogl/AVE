// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/Actor.h"
#include "BaseBital.h"

// Sets default values for this component's properties
UBaseBital::UBaseBital()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBaseBital::init(float getHP, float getPosture, float getDamage, float recv, float time)
{
	hp = hpMax = getHP;
	posture = postureMax = getPosture;
	damage = getDamage;
	postureRecv = recv;
	postureTime = time;

	GetOwner()->GetWorldTimerManager().SetTimer(tPostureTimer, this, &UBaseBital::postureRecover, postureTime, false);
}

// Called when the game starts
void UBaseBital::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UBaseBital::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bitalInfo UBaseBital::onHit(int characterDamage)
{
	hp -= characterDamage;
	if (hp <= 0)
	posture -= characterDamage;
	if (posture <= 0)

	myInfo.hp=hp;
	myInfo.hpMax=hpMax;
	myInfo.posture=posture;
	myInfo.postureMax=postureMax;

	return myInfo;
}

void UBaseBital::postureRecover()
{
	posture += postureRecv;
	if (posture >= postureMax)
		posture = postureMax;
}

