//Name: Aditya Kaliappan
//File: FrogAditya.cpp
//
//This file contains basic motion functions for the main player.
//
//References:
//http://api.unrealengine.com/INT/API/Runtime/Engine/Components/UInputComponent/BindAction/1/index.html
//https://wiki.unrealengine.com/First_Person_Shooter_C%2B%2B_Tutorial
//https://answers.unrealengine.com/questions/212373/character-rotation-problem-c.html

#include "FrogAditya.h"
#include "Components/InputComponent.h"

// Sets default values
AFrogAditya::AFrogAditya()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrogAditya::BeginPlay()
{
	Super::BeginPlay();
	isRight = false;
	moveScale = 500;
}

// Called every frame
void AFrogAditya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*float xVel = GetVelocity().X;

	if (xVel < 0) {
		isRight = false;

		FRotator flip(0.0f, 180.0f, 0.0f);
		SetActorRotation(flip);
	} else if (xVel > 0) {
		isRight = true;

		FRotator flip(0.0f, 0.0f, 0.0f);
		SetActorRotation(flip);
	}*/

	/*float xVel = GetVelocity().X;
	bool flipSprite = (xVel < 0 && isRight) || (xVel > 0 && !isRight);

	if (flipSprite) {
		FRotator flip(0.0f, 180.0f, 0.0f);
		SetActorRotation(flip);
	}*/
}

// Called to bind functionality to input
void AFrogAditya::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//bind movement functions to input controls
	PlayerInputComponent->BindAxis("LeftRight", this, &AFrogAditya::MoveLeftRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump); 
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AFrogAditya::MoveLeftRight(float dir)
{
	if (dir != 0.0f) {
		//dir = (isRight) ? dir : -dir;

		AddMovementInput(GetActorForwardVector() * moveScale, dir);
	}
}