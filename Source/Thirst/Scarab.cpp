/*Author: Jack Wang
 *File: Scarab.cpp
 *
 *This file contains the AI functionality for the scarab enemy.
 */

#include "Scarab.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

// Sets default values
AScarab::AScarab()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	isRight = true;
	isHigh = false;
}

// Called when the game starts or when spawned
void AScarab::BeginPlay()
{
	Super::BeginPlay();

	//get the position of scarab as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set scarab to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void AScarab::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		this->Destroy();
	}

	//get X component of scarab velocity
	float xVel = GetVelocity().X;

	//compute rotation for scarab to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
		GetControlRotation().Yaw);

	//rotate player based on direction of motion
	SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

	//scarab has detected player
	if (CanBite()) {
		Bite();
	}
	//scarab does not find player
	else {
		Sliding();
	}

	//set next animation state
	UPaperFlipbook* currAnim;

	//set animation to slither
	currAnim = slidingAnim;

	//update player animation if incorrect
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}
}

void AScarab::Sliding() {
	//determine moving direction
	if (isHigh) {
		if (isRight)
			AddMovementInput(FVector(speedScale, 0.0f, 0.0f), 1.4f);
		else
			AddMovementInput(FVector(speedScale, 0.0f, 0.0f), -1.4f);
	}
	else {
		if (isRight)
			AddMovementInput(FVector(speedScale, 0.0f, speedScale), 1.4f);
		else
			AddMovementInput(FVector(speedScale, 0.0f, -speedScale), -1.4f);
	}

	//AddMovementInput(FVector(speedScale, 0.0f, speedScale), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float dispX = currX - center.X;

	//get current Z position
	float currZ = GetCharacterMovement()->GetActorLocation().Z;

	//get displacement from starting point
	float dispZ = currZ - center.Z;

	//constrain scarab movement to specified radius
	if ((dispZ > moveHeight*2)|| (dispZ < 10.0f)) {
		GetCharacterMovement()->StopMovementImmediately();
		isHigh = !isHigh;
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("isHigh changed!"));
	}

	if ((isRight && dispX > moveRadius) || (!isRight && -dispX > moveRadius)) {
		GetCharacterMovement()->StopMovementImmediately();
		isRight = !isRight;
	}
		
}

float AScarab::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	return enemy->GetActorLocation().X - currX;
}

bool AScarab::CanBite() {
	//get displacement to player
	float disp = GetPlayerDisp();

	return abs(disp) <= visionDist && (isRight != (disp < 0));
}

void AScarab::Bite() {
	//update scarab to face player
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));
}

//called when an attack hits the scarab
void AScarab::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}
