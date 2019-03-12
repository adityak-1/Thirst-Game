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
//UE4 Template: SideScroller2DCharacter.h, SideScroller2DCharacter.cpp

#include "FrogAditya.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Snake.h"
#include "Engine/World.h"

// Sets default values
AFrogAditya::AFrogAditya()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	//scale the amount of gravity on player
	GetCharacterMovement()->GravityScale = gravityScale;

	//control scale of X-axis movement when falling
	GetCharacterMovement()->AirControl = airControl;

	//set the initial upward velocity when jumping
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;

	//set default friction for surface that player walks on
	GetCharacterMovement()->GroundFriction = groundFriction;

	//set maximum walking speed for player
	GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed;

	//prevents player standing off the edge of platform due to CapsuleComponent
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	

	//initialize flags for player movements
	isDash = false;
	dashAgain = true;
	isMelee = false;
	isRanged = false;
}

// Called when the game starts or when spawned
void AFrogAditya::BeginPlay()
{
	Super::BeginPlay();

	//Delegate for handling Melee hits
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFrogAditya::MeleeHit);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void AFrogAditya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//get components of player velocity
	float xVel = GetVelocity().X;
	float zVel = GetVelocity().Z;

	//compute rotation for player to face correct direction
	float yawAngle = (xVel > 0.0f && !isBackstep) ? 180.0f :
		((xVel < 0.0f && !isBackstep) ? 0.0f : GetControlRotation().Yaw);

	//rotate player based on direction of motion
	if (Controller != nullptr) {
		Controller->SetControlRotation(FRotator(0.0f, yawAngle, 0.0f));
	}

	//check whether player can dash again
	if (GetCharacterMovement()->IsMovingOnGround()) {
		dashAgain = true;
	}

	//set next animation state
	UPaperFlipbook* currAnim;

	//player is performing melee attack
	if (isMelee) {
		currAnim = meleeAnim;
	}
	//player is performing ranged attack
	else if (isRanged) {
		currAnim = rangedAnim;
	}
	//player is performing dash
	else if (isDash) {
		currAnim = dashAnim;
	}
	//player is performing backstep
	else if (isBackstep) {
		currAnim = backstepAnim;
	}
	//player is in air
	else if (!GetCharacterMovement()->IsMovingOnGround()) {
		//can only do jump of fall
		currAnim = (zVel >= 0) ? jumpAnim : fallAnim;
	}
	//player is on ground
	else {
		//can walk or stand idle
		currAnim = (xVel != 0.0f) ? walkAnim : idleAnim;
	}

	//update player animation if incorrect
	if (GetSprite()->GetFlipbook() != currAnim) {
		GetSprite()->SetFlipbook(currAnim);
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
	PlayerInputComponent->BindAction("Backstep", IE_Pressed, this, &AFrogAditya::Backstep);
	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AFrogAditya::Melee);
	PlayerInputComponent->BindAction("Ranged", IE_Pressed, this, &AFrogAditya::Ranged);
}

void AFrogAditya::MoveLeftRight(float dir)
{
	//disable left/right motion if performing dash or attack
	if (isDash || isMelee || isRanged)
		return;

	//add motion to player in X-axis based on selected direction
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), dir);
}

void AFrogAditya::Dash()
{
	//disable multiple dashes while in the air
	//also disable if doing backstep or attack
	if (!dashAgain || isDash || isBackstep || isMelee || isRanged)
		return;

	//initialize movement for dash
	isDash = true;
	dashAgain = false;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	//compute dash direction
	float dir = (GetControlRotation().Yaw == 0.0f) ? -1.0f : 1.0f;

	//begin movement for dash
	LaunchCharacter(FVector(dashVel * dir, 0.0f, 0.0f), true, true);

	//wait for some time and stop dash
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::StopDashing, dashDuration, false);
}

void AFrogAditya::StopDashing()
{
	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = gravityScale;
	isDash = false;
}

void AFrogAditya::Backstep()
{
	//do not allow overlapping backstep moves and only allow when grounded
	//also disable if doing dash on ground or attacking
	if (isBackstep || !GetCharacterMovement()->IsMovingOnGround() || isDash || isMelee || isRanged)
		return;

	//initialize movement for dash
	isBackstep = true;
	GetCharacterMovement()->JumpZVelocity = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	//compute backstep direction
	float dir = (GetControlRotation().Yaw == 0.0f) ? 1.0f : -1.0f;

	//begin movement for backstep
	LaunchCharacter(FVector(backstepVel * dir, 0.0f, 0.0f), true, true);

	//wait for some time and stop backstep
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::StopBackstep, backstepDuration, false);
}

void AFrogAditya::StopBackstep()
{
	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
	isBackstep = false;
}

//beginning of melee cycle (startup)
void AFrogAditya::Melee()
{
	//disable attacking while attacking (attack while dashing and backstepping still possible for now)
	if (isMelee || isRanged)
		return;

	isMelee = true;

	//TODO reduce water by meleeWater

	//wait for some time, activate hitbox
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::ActivateMelee, meleeStartup, false);
}

//second part of melee cycle (attack can hit)
void AFrogAditya::ActivateMelee()
{
	//TODO activate melee hitbox
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	//ECollisionEnabled::Type Collision = ECollisionEnabled::QueryOnly;
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//wait for some time, deactivate hitbox
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::DeactivateMelee, meleeActivation, false);
}

//third part of melee cycle (attack can no longer hit)
void AFrogAditya::DeactivateMelee()
{
	//deactivate melee hitbox
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	//ECollisionEnabled::Type Collision = ECollisionEnabled::NoCollision;
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//wait for some time, end attack
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::StopMelee, meleeEndlag, false);
}

//end of melee cycle (endlag is finished, player can move again)
void AFrogAditya::StopMelee()
{
	isMelee = false;
}

//beginning of ranged cycle (startup)
void AFrogAditya::Ranged()
{
	//disable attacking while attacking (attack while dashing and backstepping still possible for now)
	if (isMelee || isRanged)
		return;

	isRanged = true;

	//TODO reduce water by rangedWater

	//wait for some time, spawn projectile
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::ActivateRanged, rangedStartup, false);
}

//second part of ranged cycle (projectile spawns)
void AFrogAditya::ActivateRanged()
{
	//TODO spawn projectile

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(delayHandle, this,
		&AFrogAditya::StopRanged, rangedEndlag, false);
}

//end of ranged cycle (player can move again)
void AFrogAditya::StopRanged()
{
	isRanged = false;
}

void AFrogAditya::MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//if other actor is enemy, call damage function
	if (OtherActor->IsA<ASnake>() && !(OtherActor == this))
	{
		/*
		AController* instigator = nullptr;	//doesn't matter for now
		*/
		Cast<ASnake>(OtherActor)->Damage(2);
	}
}
