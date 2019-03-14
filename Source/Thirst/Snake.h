// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Snake.generated.h"

/**
 *
 */
UCLASS()
class THIRST_API ASnake : public APaperCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASnake();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// function to get displacement to player
	UFUNCTION()
		float GetPlayerDisp();

	// function to handle snake slither
	UFUNCTION()
		void Slither();

	// functions to handle snake lunge
	UFUNCTION()
		bool CanLunge();
	UFUNCTION()
		void Lunge();
	UFUNCTION()
		void StartLunge();
	UFUNCTION()
		void StopLunge(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

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

	//distance that snake can see ahead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float visionDist;

	//time delay before lunge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float lungeDelay;

	//x-velocity of lunge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float lungeXVel;

	//z-velocity of lunge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float lungeZVel;

	//velocity of recoil after lunge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float recoilVel;

	//animation when player is slithering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* slitherAnim;

	//animation when player is lunging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* lungeAnim;

	//health of snake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int hitPoints;

private:
	FVector center;
	APawn* enemy;
	bool isRight;
	bool isLunge;
};