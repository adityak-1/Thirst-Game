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

#include "Snake.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/GameEngine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASnake::ASnake()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	isRight = false;
	isLunge = false;
}

// Called when the game starts or when spawned
void ASnake::BeginPlay()
{
	Super::BeginPlay();

	//get the position of snake as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set snake to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//get the collision capsule on the snake
	UCapsuleComponent* collisionCapsule = GetCapsuleComponent();
	collisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &ASnake::StopLunge);
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

	//compute rotation for snake to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
		GetControlRotation().Yaw);

	//rotate player based on direction of motion
	SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

	//check if snake can start lunge
	if (CanLunge()) {
		Lunge();
	}
	//snake is not attacking player
	else if(GetCharacterMovement()->IsMovingOnGround()) {
		isLunge = false;

		Slither();
	}

	//set next animation state
	UPaperFlipbook* currAnim;

	//set animation to slither
	currAnim = isLunge ? lungeAnim : slitherAnim;

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

	return currX - enemy->GetActorLocation().X;
}

bool ASnake::CanLunge() {
	//get displacement to player
	float disp = GetPlayerDisp();

	return !isLunge && GetCharacterMovement()->IsMovingOnGround() && abs(disp) <= visionDist && (isRight == (disp < 0));
}

void ASnake::Lunge() {
	//set lunge flag to true
	isLunge = true;

	//update snake to face snake (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));

	//stop snake slither movement
	GetCharacterMovement()->StopMovementImmediately();

	FTimerHandle delayHandle;
	//wait for some time and start lunge
	GetWorld()->GetTimerManager().SetTimer(delayHandle, this,
		&ASnake::StartLunge, 0.5f, false);
}

void ASnake::StartLunge() {
	//begin movement for lunge
	LaunchCharacter(FVector(200 * (isRight ? 1.0f : -1.0f), 0.0f, 200.0f), true, true);
}

void ASnake::StopLunge(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == enemy) {
		if (OtherComp->GetName() != "MeleeCollision") {
			//decrease health
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherComp->GetName());
		}

		//recoil backwards
		GetCharacterMovement()->StopMovementImmediately();
		LaunchCharacter(FVector(300 * (isRight ? -1.0f : 1.0f), 0.0f, 0.0f), true, true);

		//set lunge flag to false
		isLunge = false;
	}
}

//called when an attack hits the snake
void ASnake::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}