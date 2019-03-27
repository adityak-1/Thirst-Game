// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <time.h>
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

	// function to handle scarab sliding
	UFUNCTION()
		void Sliding();

	// functions to handle scarab bite
	UFUNCTION()
		bool CanBite();
	void Bite();

	//function for scarab to make damage
	UFUNCTION()
		void Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//function for scarab to take damage
	UFUNCTION()
		void Damage(int damageTaken);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale for scarab speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float speedScale;

	//radius that scarab can travel within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float moveRadius;

	//distance that scarab can see ahead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float visionDist;

	//animation when player is slithering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* hoverAnim;

	//animation when player is lunging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* biteAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int hitPoints;
	//attack

private:
	FVector center;
	APawn* enemy;
	bool isRight;
	bool isBiting;
	bool currentBiting;
};
