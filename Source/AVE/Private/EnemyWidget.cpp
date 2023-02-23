// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidget.h"
#include "Components/ProgressBar.h"

void UEnemyWidget::NativeTick(const FGeometry& MyGeometry, float InDeletaTime)
{
	Super::NativeTick(MyGeometry, InDeletaTime);

	if (!OwnerEnemy.IsValid())
		return;

	HPBar->SetPercent(OwnerEnemy->GetHP() / OwnerEnemy->GetMaxHP());
	PostureBar->SetPercent(OwnerEnemy->GetPosture() / OwnerEnemy->GetMaxPosture());

	if (HPBar->Percent >= 1)
		HPBar->SetVisibility(ESlateVisibility::Hidden);
	else
		HPBar->SetVisibility(ESlateVisibility::Visible);

	if (PostureBar->Percent >= 1)
		PostureBar->SetVisibility(ESlateVisibility::Hidden);
	else
		PostureBar->SetVisibility(ESlateVisibility::Visible);
}

//float UEnemyWidget::DisplayHP_C()
//{
//	HPBar->SetPercent(HP/HPMax);
//	return HP/HPMax;
//}
//
//float UEnemyWidget::DisplayPosture_C()
//{
//	PostureBar->SetPercent(Posture / PostureMax);
//	return Posture / PostureMax;
//}
//
//void UEnemyWidget::SetInfo_C(float MaxHP, float MaxPosture)
//{
//	HPMax = MaxHP;
//	PostureMax = MaxPosture;
//}
//
//void UEnemyWidget::Refresh_C(float nowHP, float nowPosture)
//{
//	HP = nowHP;
//	Posture = nowPosture;
//	DisplayHP_C();
//	DisplayPosture_C();
//}