//Name: Aditya Kaliappan
//File: FrogAditya.cpp
//
//This file contains basic motion functions for the main player.
//
//References:
//http://api.unrealengine.com/INT/API/Runtime/Engine/Components/UInputComponent/BindAction/1/index.html
//https://wiki.unrealengine.com/First_Person_Shooter_C%2B%2B_Tutorial
//https://answers.unrealengine.com/questions/212373/character-rotation-problem-c.html
//https://api.unrealengine.com/INT/API/Runtime/Engine/GameFramework/UCharacterMovementComponent/index.html
//https://api.unrealengine.com/INT/API/Runtime/Engine/GameFramework/ACharacter/GetCharacterMovement/index.html

#include "FrogAditya.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
AFrogAditya::AFrogAditya()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//scale the amount of gravity on player
	GetCharacterMovement()->GravityScale = 1.5f;

	//control scale of X-axis movement when falling
	GetCharacterMovement()->AirControl = 0.7f;

	//set the initial upward velocity when jumping
	GetCharacterMovement()->JumpZVelocity = 700.f;
	
	//set default friction for surface that player walks on
	GetCharacterMovement()->GroundFriction = 3.0f;

	//set maximum walking speed for player
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	//prevents player standing off the edge of platform due to CapsuleComponent
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
}

// Called when the game starts or when spawned
void AFrogAditya::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFrogAditya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//get X-component of player velocity
	float xVel = GetVelocity().X;

	//compute rotation for player to face correct direction
	float yawAngle = (xVel > 0.0f) ? 180.0f : 
		((xVel < 0.0f) ? 0.0f : GetControlRotation().Yaw);

	//rotate player based on direction of motion
	if (Controller != nullptr) {
		Controller->SetControlRotation(FRotator(0.0f, yawAngle, 0.0f));
	}
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
	//add motion to player in X-axis based on selected direction
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), dir);
}