// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBar::HealthUpdate()
{
	if (!OwnerCar->IsValidLowLevel())
		return;

	HealthBar->SetPercent(OwnerCar->GetAmmo() / OwnerCar->GetMaxAmmo());
	
	FNumberFormattingOptions Opts;
	Opts.SetMaximumFractionalDigits(0);
	CurrentHealthLabel->SetText(FText::AsNumber(OwnerCar->GetAmmo(), &Opts));
	MaxHealthLabel->SetText(FText::AsNumber(OwnerCar->GetMaxAmmo(), &Opts));

}
