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
#include "Components/BoxComponent.h"
#include "Frog.h"
#include <time.h>

 // Sets default values
AScarab::AScarab()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	isRight = true;
	isBiting = false;
	currentBiting = false;
}

// Called when the game starts or when spawned
void AScarab::BeginPlay()
{
	Super::BeginPlay();

	//get the position of scarab as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set scarab to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//get the collision box on the scarab
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	if (collisionBox)
		collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AScarab::Collide);
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
	}
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
		//set next animation state
		UPaperFlipbook* currAnim;

		//set animation to hover
		currAnim = hoverAnim;

		//update player animation if incorrect
		if (GetSprite()->GetFlipbook() != currAnim) {
			GetSprite()->SetFlipbook(currAnim);
		}
		Sliding();
	}

	//currentBiting = false;
	isBiting = false;
}

void AScarab::Sliding() {

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
	if (!currentBiting) {
		UPaperFlipbook* currAnim;
		currAnim = biteAnim;
		if (GetSprite()->GetFlipbook() != currAnim) {
			GetSprite()->SetFlipbook(currAnim);
		}

		//update scarab to face player
		SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));
		startBitingTime = static_cast<long> (time(NULL));
		isBiting = true;
		//currentBiting = true;
	}
}

void AScarab::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != this) {
		//check whether scarab is biting
		if (isBiting) {

			//check if player was hit
			if (OtherActor == enemy && OtherComp->GetName() != "MeleeCollision") {
				Cast<AFrog>(OtherActor)->Damage(20);
			}
			//isBiting = false;
		}
		else {
			//flip direction of scarab
			isRight = !isRight;
		}
	}
}

//called when an attack hits the scarab
void AScarab::Damage(int damageTaken) {
	hitPoints -= 3;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("damage taken!!!!!!!!!"));
}