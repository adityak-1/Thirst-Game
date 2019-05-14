// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCustomGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Reset")
		FVector spawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Reset")
		bool showStartBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Reset")
		bool gameDone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Reset")
		bool playerDied;
};
