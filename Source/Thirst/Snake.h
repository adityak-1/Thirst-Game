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

	// functions to handle snake slither
	UFUNCTION()
		void Slither();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale for snake speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float speedScale;

	//radius that snake can travel within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float moveRadius;

	//animation when player is slithering
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* slitherAnim;

	//animation when player is lunging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* lungeAnim;

private:
	FVector center;
	bool isRight;
};
