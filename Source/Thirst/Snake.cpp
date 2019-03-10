//Name: Aditya Kaliappan
//File: Snake.cpp
//
//This file contains the AI functionality for the snake enemy.
//
//References:
//https://answers.unrealengine.com/questions/63322/how-to-get-the-player-controller-in-c.html
//https://answers.unrealengine.com/questions/664426/ugameplaystatics-is-not-a-class-or-namespace-name.html
//https://answers.unrealengine.com/questions/670373/pointer-to-incomplete-class-type-is-not-allowed-2.html

#include "Snake.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

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

	//set snake to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();
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

	//snake has detected player
	if (CanLunge()) {
		Lunge();
	}
	//snake does not find player
	else {
		Slither();
	}

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

float ASnake::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	return enemy->GetActorLocation().X - currX;
}

bool ASnake::CanLunge() {
	//get displacement to player
	float disp = GetPlayerDisp();

	return abs(disp) <= visionDist && (isRight != (disp < 0));
}

void ASnake::Lunge() {
	//update snake to face snake (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));
}