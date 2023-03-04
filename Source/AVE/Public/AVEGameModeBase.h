// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AVEGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AVE_API AAVEGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* BGM;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartSlowMotion();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EndSlowMotion();

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* MusicToPlay);

	UFUNCTION(BlueprintCallable)
	void StopBGM();
};
