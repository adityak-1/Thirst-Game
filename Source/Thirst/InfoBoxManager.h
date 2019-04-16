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

	//function to display info box
	UFUNCTION()
		void DisplayBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//function to hide info box
	UFUNCTION()
		void HideBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//info boxes in game
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information Boxes")
		TArray<TSubclassOf<class UUserWidget> > infoBoxes;

	//do each of the info boxes in game have input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Information Boxes")
		TArray<bool> infoBoxInput;

	//player in game
	APawn *player;

private:
	TArray<UUserWidget*> widgets;
};
