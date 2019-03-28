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
ASnake::ASnake()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	//set default values for flags
	isRight = false;
	beginLunge = false;
	endLunge = false;
}

// Called when the game starts or when spawned
void ASnake::BeginPlay()
{
	Super::BeginPlay();

	//get the position of snake as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set snake to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//get the collision box on the snake
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASnake::Collide);
}

// Called every frame
void ASnake::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		this->Destroy();
	}

	//get X component of snake velocity
	float xVel = GetVelocity().X;

	//determine whether snake can slither
	bool canSlither = !beginLunge && !endLunge;

	//check if snake can lunge at player
	if (CanLunge()) {
		//perform lunge
		Lunge();
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

float ASnake::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//return displacement
	return currX - enemy->GetActorLocation().X;
}

bool ASnake::CanLunge() {
	//get displacement to player
	float disp = GetPlayerDisp();

	//return whether snake can perform lunge
	return !beginLunge && !endLunge && GetCharacterMovement()->IsMovingOnGround() 
		&& abs(disp) <= visionDist && (isRight == (disp < 0));
}

void ASnake::Lunge() {
	//set lunge flag to true
	beginLunge = true;

	//update snake to face snake (if incorrect)
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
	//begin movement for lunge
	LaunchCharacter(FVector(lungeXVel * (isRight ? 1.0f : -1.0f), 0.0f, lungeZVel), true, true);

	//wait for some time and stop lunge
	GetWorld()->GetTimerManager().SetTimer(endTimer, this,
		&ASnake::StopLunge, lungeDelay, false);
}

void ASnake::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor == enemy && OtherComp->GetName() != "MeleeCollision") {
		Cast<AFrog>(OtherActor)->Damage(15);
	}

	if (OtherActor != this) {
		//check whether snake is lunging
		if (beginLunge) {
			//update lunge flags
			endLunge = true;
			beginLunge = false;

			//disable timed ending of lunge
			GetWorldTimerManager().ClearTimer(endTimer);

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

void ASnake::StopLunge() {
	//update lunge flags
	endLunge = true;
	beginLunge = false;

	//recoil backwards
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(FVector(recoilVel * (isRight ? -1.0f : 1.0f), 0.0f, 0.0f), true, true);

	//wait for some time and reset lunge
	GetWorld()->GetTimerManager().SetTimer(resetTimer, this,
		&ASnake::ResetLunge, lungeDelay, false);
}

void ASnake::ResetLunge() {
	//stop current movement
	GetCharacterMovement()->StopMovementImmediately();

	//reset snake to slither animation with looping
	GetSprite()->SetLooping(true);
	GetSprite()->Play();
	GetSprite()->SetFlipbook(slitherAnim);

	//clear lunge flags
	endLunge = false;
}

//called when an attack hits the snake
void ASnake::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}