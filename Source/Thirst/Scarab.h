// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Scarab.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API AScarab : public APaperCharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AScarab();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// function to get displacement to player
	UFUNCTION()
		float GetPlayerDisp();

	// function to handle snake sliding
	UFUNCTION()
		void Sliding();

	// functions to handle snake lunge
	UFUNCTION()
		bool CanBite();
	void Bite();

	//function for snake to take damage
	UFUNCTION()
		void Damage(int damageTaken);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale for snake speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float speedScale;

	//radius that snake can travel within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float moveRadius;

	//radius that snake can travel within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float moveHeight;

	//distance that snake can see ahead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float visionDist;

	//animation when player is slithering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* slidingAnim;

	//animation when player is lunging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* biteAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int hitPoints;

private:
	FVector center;
	APawn* enemy;
	bool isRight;
	bool isHigh;
};
