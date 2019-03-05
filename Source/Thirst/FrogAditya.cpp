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
//https://api.unrealengine.com/INT/API/Runtime/Engine/GameFramework/UNavMovementComponent/index.html
//https://www.youtube.com/watch?v=xXG-fYzpSW4&index=8&list=PLZlv_N0_O1gYup-gvJtMsgJqnEB_dGiM4

#include "FrogAditya.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"

// Sets default values
AFrogAditya::AFrogAditya()
{
	//set this character to call Tick() every frame
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

	//initialize flags for player movements
	isDash = false;
	dashAgain = true;
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

	//check whether player can dash again
	if (GetCharacterMovement()->IsMovingOnGround()) {
		dashAgain = true;
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
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFrogAditya::Dash);
}

void AFrogAditya::MoveLeftRight(float dir)
{
	//disable left/right motion if performing dash
	if (isDash)
		return ;

	//add motion to player in X-axis based on selected direction
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), dir);
}

void AFrogAditya::Dash()
{
	//disable multiple dashes while in the air
	if (!dashAgain || isDash)
		return ;

	//initialize movement for dash
	isDash = true;
	dashAgain = false;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	//compute dash direction
	float dir = (GetControlRotation().Yaw == 0.0f) ? -1.0f : 1.0f;

	//begin movement for dash
	LaunchCharacter(FVector(3000.0f * dir, 0.0f, 0.0f), true, true);

	//wait for some time and stop dash
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::StopDashing, 0.2f, false);
}

void AFrogAditya::StopDashing()
{
	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = 1.5f;
	isDash = false;
}