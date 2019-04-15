// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfoBoxManager.generated.h"

UCLASS()
class THIRST_API AInfoBoxManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInfoBoxManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//get distance to player
	float GetPlayerDisp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//info box at game start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information Boxes")
		TSubclassOf<class UUserWidget> startBox;

	//player in game
	APawn *player;
};
