// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "FrogAditya.generated.h"

/**
 *
 */
UCLASS()
class THIRST_API AFrogAditya : public APaperCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFrogAditya();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// function to handle left/right motion of player
	UFUNCTION()
		void MoveLeftRight(float val);

	// functions to handle dash motion of player
	UFUNCTION()
		void Dash();
	UFUNCTION()
		void StopDashing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool isDash;
	FTimerHandle delayHandle;
};