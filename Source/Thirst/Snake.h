// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Snake.generated.h"

/**
 *
 */
UCLASS()
class THIRST_API ASnake : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASnake();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Death() override;

	UFUNCTION()
		void DeathHelper();

	UFUNCTION()
		void DeathHelperHelper();

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
		void StopLunge(bool shouldRecoil = true);
	UFUNCTION()
		void ResetLunge();
	UFUNCTION()
		void Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

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

	//delay time for death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float deathDelay;

	//animation when player is slithering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* slitherAnim;

	//animation when player is lunging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* lungeAnim;

	//animation for death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* deathAnim;

	//animation for death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* deathStillAnim;

private:
	bool beginLunge;
	bool inLunge;
	bool endLunge;
	bool isKilled;
	FTimerHandle startTimer;
	FTimerHandle resetTimer;
	FTimerHandle deathDelayTimer;
};