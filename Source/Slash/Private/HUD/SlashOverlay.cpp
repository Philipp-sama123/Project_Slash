// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGoldCount(int32 Gold)
{
	if (GoldCountText)
	{
		GoldCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void USlashOverlay::SetSoulsCount(int32 Souls)
{
	if (SoulsCountText)
	{
		SoulsCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}
