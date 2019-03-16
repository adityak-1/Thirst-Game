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
#include "Components/BoxComponent.h"
#include "Frog.h"

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

	//get the collision box on the snake
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASnake::StopLunge);
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
		//set lunge flag to true
		isLunge = true;

		Lunge();
	}
	//snake is not attacking player
	else if(GetCharacterMovement()->IsMovingOnGround()) {
		//reset lunge flag to false
		isLunge = false;

		Slither();
	}

	//set next animation state
	UPaperFlipbook* currAnim;

	//update animation based on movement of snake
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
	//update snake to face snake (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));

	//stop snake slither movement
	GetCharacterMovement()->StopMovementImmediately();

	FTimerHandle delayHandle;
	//wait for some time and start lunge
	GetWorld()->GetTimerManager().SetTimer(delayHandle, this,
		&ASnake::StartLunge, lungeDelay, false);
}

void ASnake::StartLunge() {
	//begin movement for lunge
	LaunchCharacter(FVector(lungeXVel * (isRight ? 1.0f : -1.0f), 0.0f, lungeZVel), true, true);
}

void ASnake::StopLunge(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherComp->GetName());
	if (OtherActor != this) {
		//check if player was hit
		if (OtherActor == enemy && OtherComp->GetName() != "MeleeCollision") {
			Cast<AFrog>(OtherActor)->Damage(2);
		}

		//recoil backwards
		GetCharacterMovement()->StopMovementImmediately();
		LaunchCharacter(FVector(recoilVel * (isRight ? -1.0f : 1.0f), 0.0f, 0.0f), true, true);

		//set lunge flag to false
		isLunge = false;
	}
}

//called when an attack hits the snake
void ASnake::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}