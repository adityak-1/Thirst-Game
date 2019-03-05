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

	//scale gravity on player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityScale;

	//scale X-axis movement when falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float airControl;

	//initial upward velocity when jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float jumpZVelocity;

	//default friction for surface player walks on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float groundFriction;

	//maximum walking speed for player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float maxWalkSpeed;

	//launch velocity for dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashVel;

	//duration of dash (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashDuration;

	//animation when player is idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleAnim;

	//animation when player is walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkAnim;

private:
	bool isDash, dashAgain;
	FTimerHandle delayHandle;
};