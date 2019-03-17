//Name: Aditya Kaliappan
//File: Lizard.cpp
//
//This file contains the AI functionality for the lizard enemy.
//
//References:

#include "Lizard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

// Sets default values
ALizard::ALizard()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	//set default values for flags
	isRight = false;
}

// Called when the game starts or when spawned
void ALizard::BeginPlay()
{
	Super::BeginPlay();

	//get the position of lizard as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set lizard to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void ALizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		this->Destroy();
	}

	//get X component of lizard velocity
	float xVel = GetVelocity().X;

	//compute rotation for lizard to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
		GetControlRotation().Yaw);

	//rotate player based on direction of motion
	SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

	//update animation to walk if not already as such
	if (GetSprite()->GetFlipbook() != walkAnim) {
		GetSprite()->SetFlipbook(walkAnim);
	}

	//perform slither
	Walk();
}

void ALizard::Walk() {
	//move lizard slightly in current frame
	AddMovementInput(FVector(speedScale, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float disp = currX - center.X;

	//bound lizard movement within radius
	if ((isRight && disp > moveRadius) || (!isRight && -disp > moveRadius)) {
		//stop current movement
		GetCharacterMovement()->StopMovementImmediately();

		//flip lizard direction
		isRight = !isRight;
	}
}

float ALizard::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//return displacement
	return currX - enemy->GetActorLocation().X;
}

//called when an attack hits the lizard
void ALizard::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}