//Name: Aditya Kaliappan
//File: Lizard.cpp
//
//This file contains the AI functionality for the lizard enemy.
//
//References:
//https://wiki.unrealengine.com/Function_Pointers

#include "Lizard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Projectile.h"

// Sets default values
ALizard::ALizard()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	//set default values for flags
	isRight = false;
	isAttack = false;
}

// Called when the game starts or when spawned
void ALizard::BeginPlay()
{
	Super::BeginPlay();

	//get the position of lizard as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set lizard to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//initialize parameters for projectile spear (if it exists)
	FActorSpawnParameters params;
	params.Owner = this;

	//spawn projectile
	spear = GetWorld()->SpawnActor<AProjectile>(spearProjectile, GetActorLocation(), GetActorRotation(), params);
	spear->Start(this, isRight, false);
}

// Called every frame
void ALizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		if (spear) {
			spear->Remove();
		}

		this->Destroy();
	}

	if (hasSpear && spear) {
		spear->UpdatePosition(isRight);
	}

	//lizard can attack
	if (CanAttack()) {
		Attack();
	}
	//lizard not near player to attack
	else if (!isAttack) {
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

		Walk();
	}
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

bool ALizard::CanAttack() {
	//get displacement to player
	float disp = GetPlayerDisp();

	//return whether lizard can perform attack
	return !isAttack && abs(disp) <= visionDist && (isRight == (disp < 0));
}

void ALizard::Attack() {
	//update attack flag to true
	isAttack = true;

	//update lizard to face player (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));

	//stop lizard walk movement
	GetCharacterMovement()->StopMovementImmediately();

	//select attack to make
	int randInt = 0;

	FunctionPtr ptr = (randInt == 0) ? &ALizard::Spit :
		(hasSpear ? &ALizard::Spear : 
		(hasDagger ? &ALizard::Dagger : &ALizard::Spit));

	//wait for some time and start attack
	GetWorld()->GetTimerManager().SetTimer(startTimer, this,
		ptr, attackDelay, false);
}

void ALizard::Spit() {
	//initialize parameters for projectile spawn
	FActorSpawnParameters params;
	params.Owner = this;

	//spawn projectile
	AProjectile* waterBall = GetWorld()->SpawnActor<AProjectile>(projectile, GetActorLocation(), GetActorRotation(), params);

	//allow the projectile to move
	waterBall->Start(this, isRight);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, resetDelay, false);
}

void ALizard::Spear() {
	//select dash or throw

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, resetDelay, false);
}

void ALizard::Dagger() {
	//dash

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, resetDelay, false);
}

void ALizard::ResetAttack() {
	//reset attack flag to false
	isAttack = false;
}

//called when an attack hits the lizard
void ALizard::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}