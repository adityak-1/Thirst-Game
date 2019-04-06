//Name: Aditya Kaliappan
//File: Frog.cpp
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
//https://www.youtube.com/watch?time_continue=3&v=Z-mbe4k5Wa4
//UE4 Template: SideScroller2DCharacter.h, SideScroller2DCharacter.cpp

#include "Frog.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Enemy.h"
#include "Engine/World.h"
#include "Projectile.h"
#include "Well.h"
#include "Runtime/Engine/Classes/GameFramework/GameModeBase.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Shade.h"

// Sets default values
AFrog::AFrog()
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
	isShaded = false;
	isStun = false;
}

// Called when the game starts or when spawned
void AFrog::BeginPlay()
{
	Super::BeginPlay();

	//Delegate for handling Melee hits
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFrog::MeleeHit);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetVisibility(false);

	//Using the capsule was causing UE4 Editor to crash
	//TODO make this work with capsule so we don't have to use an absurd extra collision box
	//Delegate for handling Well overlap
	UBoxComponent* WellBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("WellCollision")));
	WellBox->OnComponentBeginOverlap.AddDynamic(this, &AFrog::WellRestore);

	//Delegates for handling when player enters and exits shade
	UBoxComponent* ShadeBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("ShadeCollision")));
	ShadeBox->OnComponentBeginOverlap.AddDynamic(this, &AFrog::InShade);
	ShadeBox->OnComponentEndOverlap.AddDynamic(this, &AFrog::OutShade);

	currentHealth = maxHealth;
	currentWater = maxWater;
	numLives -= 1;
	checkPoint = NULL;
}

// Called every frame
void AFrog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!isShaded) {
		currentWater -= tickWater;
	}

	if (currentWater <= 0) {
		currentWater = 0;
		currentHealth -= tickHealth;
	}
	
	if (currentHealth <= 0)
		Die();

	//get components of player velocity
	float xVel = GetVelocity().X;
	float zVel = GetVelocity().Z;

	//compute rotation for player to face correct direction
	float yawAngle = (xVel > 0.0f && !isBackstep) ? 0.0f :
		((xVel < 0.0f && !isBackstep) ? 180.0f : GetControlRotation().Yaw);

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
void AFrog::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//bind movement functions to input controls
	PlayerInputComponent->BindAxis("LeftRight", this, &AFrog::MoveLeftRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFrog::Dash);
	PlayerInputComponent->BindAction("Backstep", IE_Pressed, this, &AFrog::Backstep);
	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AFrog::Melee);
	PlayerInputComponent->BindAction("Ranged", IE_Pressed, this, &AFrog::Ranged);
}

void AFrog::MoveLeftRight(float dir)
{
	//disable left/right motion if performing other movement or attack
	if (isDash || isBackstep || isStun || isMelee || isRanged)
		return;

	//add motion to player in X-axis based on selected direction
	AddMovementInput(FVector(0.5f, 0.0f, 0.0f), dir);
}

void AFrog::Dash()
{
	//disable multiple dashes while in the air
	//also disable if doing backstep or attack
	if (!dashAgain || isDash || isBackstep || isMelee || isRanged || isStun)
		return;

	//reduce player's water meter
	currentWater -= dashWater;

	//initialize movement for dash
	isDash = true;
	dashAgain = false;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	//compute dash direction
	float dir = (GetControlRotation().Yaw == 180.0f) ? -1.0f : 1.0f;

	//begin movement for dash
	LaunchCharacter(FVector(dashVel * dir, 0.0f, 0.0f), true, true);

	//wait for some time and stop dash
	GetWorldTimerManager().SetTimer(movementTimer, this,
		&AFrog::StopDashing, dashDuration, false);
}

void AFrog::StopDashing()
{
	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->GravityScale = gravityScale;
	isDash = false;
}

void AFrog::Backstep()
{
	//do not allow overlapping backstep moves and only allow when grounded
	//also disable if doing dash on ground or attacking
	if (!isStun && (isBackstep || !GetCharacterMovement()->IsMovingOnGround() || isDash || isMelee || isRanged))
		return;

	//initialize movement for dash
	isBackstep = true;
	GetCharacterMovement()->JumpZVelocity = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();

	//compute backstep direction
	float dir = (GetControlRotation().Yaw == 180.0f) ? 1.0f : -1.0f;

	//begin movement for backstep
	LaunchCharacter(FVector(backstepVel * dir, 0.0f, 0.0f), true, true);

	//wait for some time and stop backstep
	GetWorldTimerManager().SetTimer(movementTimer, this,
		&AFrog::StopBackstep, backstepDuration, false);
}

void AFrog::StopBackstep()
{
	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
	isBackstep = false;
	isStun = false;
}

//beginning of melee cycle (startup)
void AFrog::Melee()
{
	//disable attacking while attacking (attack while dashing and backstepping still possible for now)
	if (isMelee || isRanged)
		return;

	isMelee = true;

	//reduce player's water meter
	currentWater -= meleeWater;

	//wait for some time, activate hitbox
	GetWorldTimerManager().SetTimer(attackTimer, this,
		&AFrog::ActivateMelee, meleeStartup, false);
}

//second part of melee cycle (attack can hit)
void AFrog::ActivateMelee()
{
	//TODO activate melee hitbox
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetVisibility(true);

	//wait for some time, deactivate hitbox
	GetWorldTimerManager().SetTimer(attackTimer, this,
		&AFrog::DeactivateMelee, meleeActivation, false);
}

//third part of melee cycle (attack can no longer hit)
void AFrog::DeactivateMelee()
{
	//deactivate melee hitbox
	UBoxComponent* CollisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("MeleeCollision")));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetVisibility(false);

	//wait for some time, end attack
	GetWorldTimerManager().SetTimer(attackTimer, this,
		&AFrog::StopMelee, meleeEndlag, false);
}

//end of melee cycle (endlag is finished, player can move again)
void AFrog::StopMelee()
{
	isMelee = false;
}

//beginning of ranged cycle (startup)
void AFrog::Ranged()
{
	//disable attacking while attacking (attack while dashing and backstepping still possible for now)
	if (isMelee || isRanged)
		return;

	isRanged = true;

	//reduce player's water meter
	currentWater -= rangedWater;

	//wait for some time, spawn projectile
	GetWorldTimerManager().SetTimer(attackTimer, this,
		&AFrog::ActivateRanged, rangedStartup, false);
}

//second part of ranged cycle (projectile spawns)
void AFrog::ActivateRanged()
{
	//initialize parameters for projectile spawn
	FActorSpawnParameters params;
	params.Owner = this;

	//spawn projectile
	AProjectile* waterBall = GetWorld()->SpawnActor<AProjectile>(projectile, GetActorLocation(), GetActorRotation(), params);
	
	//allow the projectile to move
	waterBall->Start(this, GetActorRotation().Yaw == 0.0f);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(attackTimer, this,
		&AFrog::StopRanged, rangedEndlag, false);
}

//end of ranged cycle (player can move again)
void AFrog::StopRanged()
{
	isRanged = false;
}

//called when a melee attack hits
void AFrog::MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//if other actor is enemy, call damage function
	if (OtherActor != this && OtherActor->IsA<AEnemy>()) {
		Cast<AEnemy>(OtherActor)->Damage(meleeDamage);
	}
}

//called when frog is hit by an enemy
void AFrog::Damage(float damageTaken, float disp) {
	//update player health
	SetCurrentHealth(currentHealth - damageTaken);

	//recoil back when damage is taken
	ResetMovement();

	//set stun flag to true
	isStun = true;

	//update player rotation so that player backsteps away from enemy
	if (Controller != nullptr) {
		Controller->SetControlRotation(FRotator(0.0f, (disp >= 0 ? 0.0f : 180.0f), 0.0f));
	}

	//backstep away from enemy
	Backstep();
}

void AFrog::ResetMovement() {
	//disable any existing movement timer
	GetWorldTimerManager().ClearTimer(movementTimer);

	//reset default player movements
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
	GetCharacterMovement()->GravityScale = gravityScale;
	isBackstep = false;
	isDash = false;
}

void AFrog::WellRestore(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (OtherActor->IsA<AWell>()) {
		SetCurrentWater(maxWater);
		SetCurrentHealth(maxHealth);
		checkPoint = OtherActor;
	}
}

void AFrog::InShade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (OtherActor->IsA<AShade>()) {
		isShaded = true;
	}
}

void AFrog::OutShade(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA<AShade>()) {
		isShaded = false;
	}
}

void AFrog::AddWater(float amount) {
	currentWater += amount;
}

//called when health <= 0
//TODO go to death screen --> respawn at last checkpoint
void AFrog::Die() {

	if (numLives <= 0) {
		Destroy();

		//game over screen

	}
	else {
		//death screen

		GetCharacterMovement()->StopMovementImmediately();
		
		//restore health and water, reduce lives.
		currentHealth = maxHealth;
		currentWater = maxWater;
		numLives -= 1;

		//restart game
		if (checkPoint == NULL) {
			//restart at PlayerStart
			GetWorld()->GetFirstPlayerController()->ClientSetLocation(GetWorld()->GetFirstPlayerController()->GetSpawnLocation(), FRotator());
		}
		else {
			//restart at last check point
			GetWorld()->GetFirstPlayerController()->ClientSetLocation(checkPoint->GetActorLocation(), FRotator());
		}
	}
}