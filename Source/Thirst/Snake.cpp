//Name: Aditya Kaliappan
//File: Snake.cpp
//
//This file contains the AI functionality for the snake enemy.
//
//References:
//https://answers.unrealengine.com/questions/63322/how-to-get-the-player-controller-in-c.html
//https://answers.unrealengine.com/questions/664426/ugameplaystatics-is-not-a-class-or-namespace-name.html
//https://answers.unrealengine.com/questions/670373/pointer-to-incomplete-class-type-is-not-allowed-2.html
//https://unrealcpp.com/on-overlap-begin/
//https://answers.unrealengine.com/questions/163258/looping-and-non-looping-flipbooks.html

#include "Snake.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Frog.h"

// Sets default values
ASnake::ASnake() : AEnemy()
{
	//set default values for flags
	isRight = false;
	beginLunge = false;
	inLunge = false;
	endLunge = false;
	isKilled = false;
}

// Called when the game starts or when spawned
void ASnake::BeginPlay()
{
	Super::BeginPlay();

	//get the collision box on the snake
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASnake::Collide);
}

// Called every frame
void ASnake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isKilled) {
		//get X component of snake velocity
		float xVel = GetVelocity().X;

		//determine whether snake can slither
		bool canSlither = !beginLunge && !endLunge;

		//check if snake can lunge at player
		if (CanLunge()) {
			//perform lunge
			Lunge();
		}
		//check if snake has landed back on ground after lunge
		else if (inLunge && GetCharacterMovement()->IsMovingOnGround()) {
			//end lunge
			StopLunge(false);
		}
		//slither if snake is not in middle of lunge
		else if (canSlither) {
			//compute rotation for snake to face correct direction
			float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
				GetControlRotation().Yaw);

			//rotate player based on direction of motion
			SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

			//update animation to slither if not already as such
			if (GetSprite()->GetFlipbook() != slitherAnim) {
				GetSprite()->SetFlipbook(slitherAnim);
			}

			//perform slither
			Slither();
		}
	}
}

void ASnake::Death() {
	//change hitpoints to avoid call death multi times
	hitPoints = 1000;

	//set isKilled to true
	isKilled = true;

	//change animation and walking method
	GetCharacterMovement()->StopMovementImmediately();
	GetSprite()->SetFlipbook(deathAnim);

	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ASnake::DeathHelper, 1.1, false);
}

void ASnake::DeathHelper() {
	GetSprite()->SetFlipbook(deathStillAnim);
	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ASnake::DeathHelperHelper, deathDelay, false);
}

void ASnake::DeathHelperHelper() {
	this->Destroy();
}

void ASnake::Slither() {
	//move snake slightly in current frame
	AddMovementInput(FVector(speedScale, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float disp = currX - center.X;

	//bound snake movement within radius
	if ((isRight && disp > moveRadius) || (!isRight && -disp > moveRadius)) {
		//stop current movement
		GetCharacterMovement()->StopMovementImmediately();

		//flip snake direction
		isRight = !isRight;
	}
}

bool ASnake::CanLunge() {
	//get displacement to player
	float disp = GetPlayerDisp();

	//return whether snake can perform lunge
	return !beginLunge && !inLunge && !endLunge && GetCharacterMovement()->IsMovingOnGround() 
		&& abs(disp) <= visionDist && (isRight == (disp < 0));
}

void ASnake::Lunge() {
	//set lunge flag to true
	beginLunge = true;

	//update snake to face player (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));

	//stop snake slither movement
	GetCharacterMovement()->StopMovementImmediately();

	//only allow lunge animation once
	GetSprite()->SetLooping(false);
	GetSprite()->SetFlipbook(lungeAnim);

	//wait for some time and start lunge
	GetWorld()->GetTimerManager().SetTimer(startTimer, this,
		&ASnake::StartLunge, lungeDelay, false);
}

void ASnake::StartLunge() {
	if (!isKilled) {
		//begin movement for lunge
		LaunchCharacter(FVector(lungeXVel * (isRight ? 1.0f : -1.0f), 0.0f, lungeZVel), true, true);

		//set flag to indicate that lunge is in progress
		inLunge = true;
	}
}

void ASnake::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	
	//check if this object is killed
	if (!isKilled) {
		//check if player was hit
		if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
			Cast<AFrog>(OtherActor)->Damage(5, GetPlayerDisp());
		}

		if (OtherActor != this) {
			//check whether snake is lunging
			if (beginLunge) {
				//allow snake to recoil
				StopLunge();
			}
			//snake is not lunging
			else {
				//flip direction of snake
				isRight = !isRight;
			}
		}
	}

}

void ASnake::StopLunge(bool shouldRecoil) {
	//update lunge flags
	endLunge = true;
	inLunge = false;
	beginLunge = false;

	//stop snake movement
	GetCharacterMovement()->StopMovementImmediately();

	//recoil backwards as needed
	if (shouldRecoil) {
		LaunchCharacter(FVector(recoilVel * (isRight ? -1.0f : 1.0f), 0.0f, 0.0f), true, true);
	}

	//wait for some time and reset lunge
	GetWorld()->GetTimerManager().SetTimer(resetTimer, this,
		&ASnake::ResetLunge, lungeDelay, false);
}

void ASnake::ResetLunge() {
	if (!isKilled) {
		//stop current movement
		GetCharacterMovement()->StopMovementImmediately();

		//reset snake to slither animation with looping
		GetSprite()->SetLooping(true);
		GetSprite()->Play();
		GetSprite()->SetFlipbook(slitherAnim);

		//clear lunge flags
		endLunge = false;
	}
}