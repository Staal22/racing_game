// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountdownWidget.generated.h"


UCLASS()
class RACING_GAME_API UCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentCount;

private:
	UPROPERTY()
	class Aracing_gameGameModeBase* RacingGameMode;
	
public:
	UFUNCTION()
	void CountdownUpdate();
	
};
