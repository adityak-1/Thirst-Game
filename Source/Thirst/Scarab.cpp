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
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Frog.h"
#include "Boss.h"

 // Sets default values
AScarab::AScarab() : AEnemy()
{
	//set default values for flags
	isRight = true;
	isBiting = false;
	currentBiting = false;
	validBite = true;
	isFlying = false;
}

// Called when the game starts or when spawned
void AScarab::BeginPlay()
{
	Super::BeginPlay();

	//get the collision box on the scarab
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->AttachToComponent(GetSprite(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Socket_0");
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AScarab::Collide);
}

// Called every frame
void AScarab::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//get X component of scarab velocity
	float xVel = GetVelocity().X;

	if (isFlying) {
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
			//set next animation state
			UPaperFlipbook* currAnim;

			//set animation to hover
			currAnim = hoverAnim;

			//update player animation if incorrect
			if (GetSprite()->GetFlipbook() != currAnim) {
				GetSprite()->SetFlipbook(currAnim);
			}
			Hover();
		}
	}
	else {
		if (CanSee()) {
			//move up and play lift up animation
			UPaperFlipbook* currAnim;

			//set animation to hover
			currAnim = liftUpAnim;

			//update player animation if incorrect
			if (GetSprite()->GetFlipbook() != currAnim) {
				GetSprite()->SetFlipbook(currAnim);
			}
			LiftUp();	
		}
		// else stay there
	}
}

void AScarab::Death()
{
	//check if boss spawned the scarab
	if (GetOwner() != NULL && GetOwner()->IsA<ABoss>()) {
		//add spawn point back for boss to reuse
		Cast<ABoss>(GetOwner())->AddSpawnPoint(FCString::Atoi(*GetName()));
	}

	// Killing reward
}

void AScarab::LiftUp() {
	AddMovementInput(FVector(0.0f, 0.0f, flyingHeight/13.0), 1.0f);

	float currZ = GetCharacterMovement()->GetActorLocation().Z;

	//get displacement from starting point
	float dispZ = currZ - center.Z;

	if (dispZ > flyingHeight) {
		GetCharacterMovement()->StopMovementImmediately();
		isFlying = true;
	}
}

void AScarab::Hover() {

	AddMovementInput(FVector(speedScale, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float dispX = currX - center.X;

	//constrain scarab movement to specified radius

	if ((isRight && dispX > moveRadius) || (!isRight && -dispX > moveRadius)) {
		GetCharacterMovement()->StopMovementImmediately();
		isRight = !isRight;
	}

}

bool AScarab::CanBite() {
	//get displacement to player
	float disp = GetPlayerDisp();

	return abs(disp) <= biteDist && (isRight != (disp < 0));
}

bool AScarab::CanSee() {
	//get displacement to player
	float disp = GetPlayerDisp();

	return abs(disp) <= visionDist;
}

void AScarab::Bite() {
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));
	AddMovementInput(FVector(speedScale/5.0, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));
	if (!currentBiting) {
		float disp = GetPlayerDisp();
		if (disp < 0)
			isRight = false;
		else
			isRight = true;
		UPaperFlipbook* currAnim;
		currAnim = biteAnim;
		if (GetSprite()->GetFlipbook() != currAnim) {
			GetSprite()->SetFlipbook(currAnim);
		}

		//update scarab to face player
		isBiting = true;
		currentBiting = true;
		GetWorld()->GetTimerManager().SetTimer(startTimer, this,
			&AScarab::BiteEnd, biteDelay, false);
	}
}

void AScarab::BiteEnd() {
	currentBiting = false;
	isRight = !isRight;

	float disp = GetPlayerDisp();
	if (disp < 0)
		isRight = false;
	else
		isRight = true;

	validBite = true;

	currentBiting = false;

	GetWorld()->GetTimerManager().SetTimer(endTimer, this,
		&AScarab::resetBite, biteDelay, false);
}

void AScarab::resetBite() {

	GetCharacterMovement()->StopMovementImmediately();

	//reset snake to slither animation with looping
	GetSprite()->SetLooping(true);
	GetSprite()->Play();
	GetSprite()->SetFlipbook(hoverAnim);

	currentBiting = false;
}

void AScarab::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
		//currentBiting = false;
		if (validBite) {
			Cast<AFrog>(OtherActor)->Damage(5, GetPlayerDisp());
			validBite = false;
		}
	}

	if (OtherActor == this) {
		isRight = !isRight;
	}
}