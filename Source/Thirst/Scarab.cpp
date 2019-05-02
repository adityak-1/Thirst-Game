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
#include "Egg.h"

 // Sets default values
AScarab::AScarab() : AEnemy()
{
	//set default values for flags
	isRight = true;
	isBiting = false;
	isBorning = false;
	isPostBiting = false;
	isBiteDelaying = false;
	isFlying = false;
	isFlyingUp = false;
	isKilled = false;
	isPostBitingCalled = false;
}

// Called when the game starts or when spawned
void AScarab::BeginPlay()
{
	Super::BeginPlay();

	//get the collision box on the scarab
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AScarab::Collide);
}

// Called every frame
void AScarab::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isKilled) {

		if (!isFlyingUp) {

			if (!isBiting) {

				if (isFlying) {
					if (CanBite()) {
						if (isBiteDelaying) {
							//get X component of scarab velocity
							float xVel = GetVelocity().X;
							//compute rotation for scarab to face correct direction
							float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
								GetControlRotation().Yaw);

							//rotate player based on direction of motion
							SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

							Hover();
						}
						else {
							GetCharacterMovement()->StopMovementImmediately();
							biteDispX = GetActorLocation().X - enemy->GetActorLocation().X;
							biteDispZ = GetActorLocation().Z - enemy->GetActorLocation().Z;
							targetZ = enemy->GetActorLocation().Z;
							targetX = enemy->GetActorLocation().X;
							isBiting = true;
							Bite();
						}
					}
					else {
						//get X component of scarab velocity
						float xVel = GetVelocity().X;
						//compute rotation for scarab to face correct direction
						float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
							GetControlRotation().Yaw);

						//rotate player based on direction of motion
						SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

						Hover();
					}
				}
				else {
					//born and fly
					if (CanSee()) {
						isBorning = true;
						Born();
					}
					//else stay there
				}
			}
			else {
				//bite and postBite
				if (!isPostBiting) {
					Bite();
				}
				else if(!isPostBitingCalled) {
					GetCharacterMovement()->StopMovementImmediately();
					BiteEnd();
				}
				else {
					BiteEnd();
				}
			}
			
		}
		else {
			//born and fly
			if (isBorning) {
				Born();
			}
			else {
				//flyingUp
				LiftUp();
			}
		}
		
	}
	else {
		//if it become a water droping to the ground
		if (GetSprite()->GetFlipbook() == dropingWaterAnim) {
			AddMovementInput(FVector(0.0f, 0.0f, dropSpeedScale), -1.0f);

		}
	}
}

void AScarab::Death()
{
	//change hitpoints to avoid call death multi times
	hitPoints = 1000;

	//set isKilled to true
	isKilled = true;

	//change animation and walking method
	GetCharacterMovement()->StopMovementImmediately();
	GetSprite()->SetFlipbook(deathAnim);

	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&AScarab::DeathToWater, deathDelay, false);

	//check if boss spawned the scarab
	/*
	if (GetOwner() != NULL && GetOwner()->IsA<ABoss>()) {
		GetWorld()->GetTimerManager().SetTimer(deathHelperTimer, this,
			&AScarab::DeathHelper, BossScarabRewardTime, false);
	}
	*/
}

void AScarab::DeathHelper() {
	//add spawn point back for boss to reuse
	//Cast<ABoss>(GetOwner())->AddSpawnPoint(FCString::Atoi(*GetName()));
	//destory current object
	this->Destroy();
}

void AScarab::DeathToWater() {
	GetSprite()->SetFlipbook(dropingWaterAnim);
}

void AScarab::Born() {
	isFlyingUp = true;

	/*//play born animation
	UPaperFlipbook* currAnim;
	currAnim = BornAnim;

	//update player animation if incorrect
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}
	
	//after born, flying to the air
	GetWorld()->GetTimerManager().SetTimer(bornDelayTimer, this,
		&AScarab::BornHelper, bornDelay, false);*/
	BornHelper();
}

void AScarab::BornHelper() {
	isBorning = false;
}

void AScarab::LiftUp() {

	UPaperFlipbook* currAnim;
	currAnim = liftUpAnim;

	//update player animation if incorrect
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}

	AddMovementInput(FVector(0.0f, 0.0f, flyingHeight / 13.0), 1.0f);
	float currZ = GetCharacterMovement()->GetActorLocation().Z;
	//get displacement from starting point
	float dispZ = currZ - center.Z;
	
	if (dispZ > flyingHeight) {
		GetCharacterMovement()->StopMovementImmediately();
		isFlying = true;
		isFlyingUp = false;
	}
}

void AScarab::Hover() {
	UPaperFlipbook* currAnim;
	currAnim = hoverAnim;
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}

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
	float currX = GetCharacterMovement()->GetActorLocation().X;
	float dispX = currX - enemy->GetActorLocation().X;

	float currZ = GetCharacterMovement()->GetActorLocation().Z;
	float dispZ = currZ - enemy->GetActorLocation().Z;

	return abs(dispX) <= biteDist && isRight == (dispX < 0) && abs(dispZ) <= biteHeight;
}

bool AScarab::CanSee() {
	//get displacement to player
	float currX = GetCharacterMovement()->GetActorLocation().X;
	float dispX = currX - enemy->GetActorLocation().X;

	float currZ = GetCharacterMovement()->GetActorLocation().Z;
	float dispZ = currZ - enemy->GetActorLocation().Z;

	return abs(dispX) <= visionDist && abs(dispZ) <= visionHeight;
}

void AScarab::Bite() {
	
	AddMovementInput(FVector(-biteDispX/6, 0.0f, -biteDispZ/6), 1.0f);

	//get X component of scarab velocity
	float xVel = GetVelocity().X;
	//compute rotation for scarab to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
		GetControlRotation().Yaw);

	//rotate player based on direction of motion
	SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

	UPaperFlipbook* currAnim;
	currAnim = biteAnim;
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}

	if (GetCharacterMovement()->GetActorLocation().Z - targetZ < biteAdjustment) {
		GetCharacterMovement()->StopMovementImmediately();
		isPostBiting = true;
	}

	if (abs(GetCharacterMovement()->GetActorLocation().Z - targetX) < 10) {
		GetCharacterMovement()->StopMovementImmediately();
		isPostBiting = true;
	}
}

void AScarab::BiteEnd() {
	if(!isPostBitingCalled)
		isPostBitingCalled = true;
	AddMovementInput(FVector(-biteDispX/6, 0.0f, biteDispZ/6), 1.0f);
	UPaperFlipbook* currAnim;
	currAnim = postBiteAnim;
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
	}

	if ((GetCharacterMovement()->GetActorLocation().Z - center.Z) > flyingHeight || GetVelocity().Z < 0) {
		GetCharacterMovement()->StopMovementImmediately();
		isBiting = false;
		isPostBiting = false;
		isPostBitingCalled = false;
		isBiteDelaying = true;
		GetWorld()->GetTimerManager().SetTimer(biteDelayTimer, this,
			&AScarab::ResetBite, resetBiteDelay, false);
	}
}

void AScarab::ResetBite() {
	isBiteDelaying = false;
}

void AScarab::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (!isKilled) {
		if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
			Cast<AFrog>(OtherActor)->Damage(5, GetPlayerDisp());
			if (isBiting) {
				GetCharacterMovement()->StopMovementImmediately();
				isPostBiting = true;
			}
		}
		else if (OtherActor->IsA<AEgg>()) {
			Born();
		}

		if (OtherActor != enemy) {
			isRight = !isRight;
		}
	}
	else {
		if (OtherActor == enemy) {
			//after death animation played to add CurrentWater
			if (GetSprite()->GetFlipbook() == dropingWaterAnim) {
				((AFrog*)OtherActor)->AddWater(healAmount);
				if (GetOwner() != NULL && GetOwner()->IsA<ABoss>()) {
					Cast<ABoss>(GetOwner())->AddSpawnPoint(FCString::Atoi(*GetName()));
					//DeathHelper();
				}
				DeathHelper();
			}
		}
	}

}