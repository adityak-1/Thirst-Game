//Name: Aditya Kaliappan
//File: Snake.cpp
//
//This file contains the AI functionality for the snake enemy.

#include "Snake.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/GameEngine.h"

// Sets default values
ASnake::ASnake()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	isRight = true;
}

// Called when the game starts or when spawned
void ASnake::BeginPlay()
{
	Super::BeginPlay();

	//get the position of snake as movement center
	center = GetCharacterMovement()->GetActorLocation();
}

// Called every frame
void ASnake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//get X component of snake velocity
	float xVel = GetVelocity().X;

	//compute rotation for snake to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
		GetControlRotation().Yaw);

	//rotate player based on direction of motion
	SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));
	
	//update player movement
	Slither();

	//set next animation state
	UPaperFlipbook* currAnim;

	//set animation to slither
	currAnim = slitherAnim;

	//update player animation if incorrect
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}
}

void ASnake::Slither() {
	//move snake slightly in current frame
	AddMovementInput(FVector(speedScale, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float disp = currX - center.X;

	//constrain snake movement to specified radius
	if ((isRight && disp > moveRadius) || (!isRight && -disp > moveRadius)) {
		GetCharacterMovement()->StopMovementImmediately();
		isRight = !isRight;
	}
}