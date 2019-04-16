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

	//function for scarab to make damage
	UFUNCTION()
		void DisplayBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//info box at game start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information Boxes")
		TSubclassOf<class UUserWidget> startBox;

	//player in game
	APawn *player;
};
