// Copyright Epic Games, Inc. All Rights Reserved.


#include "AVEGameModeBase.h"
#include <Components/AudioComponent.h>
#include <Kismet/GameplayStatics.h>

void AAVEGameModeBase::PlayBGM(USoundBase* MusicToPlay)
{
	if (BGM)
	{
		BGM->Stop();
	}
	BGM = UGameplayStatics::SpawnSound2D(this, MusicToPlay);
}

void AAVEGameModeBase::StopBGM()
{
	if (BGM)
	{
		BGM->Stop();
	}
	BGM = nullptr;
}
