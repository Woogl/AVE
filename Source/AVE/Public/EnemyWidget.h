// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "EnemyWidget.generated.h"

UCLASS(BlueprintType)
class AVE_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HPMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Posture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PostureMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (BindWidget))
	class UProgressBar* HPBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* PostureBar;

	//UFUNCTION(BlueprintImplementableEvent/*, meta = (DisplayName = "HPBar")*/)
	//float  DisplayHP();
	//UFUNCTION(BlueprintImplementableEvent/*, meta = (DisplayName = "PostureBar")*/)
	//float  DisplayPosture();
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetInfo(float MaxHP, float MaxPosture);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)  
	void Refresh(float nowHP, float nowPosture);
};
