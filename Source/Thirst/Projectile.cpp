//Name: Aditya Kaliappan
//File: Projectile.cpp
//
//This file contains basic motion functions for a projectile.

#include "Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Frog.h"
#include "Enemy.h"

// Sets default values
AProjectile::AProjectile() {
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	//set initial gravity of projectile to 0
	GetCharacterMovement()->GravityScale = 0.0f;

	//set moving flag to false
	isMoving = false;
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	//rotate projectile to follow arch
	if (zVelocity != 0.0f && GetVelocity().Z != 0.0f && GetVelocity().X != 0.0f) {
		//get current rotation
		FRotator rotation = GetActorRotation();

		//update rotation based on velocity in arch
		rotation.Pitch = GetVelocity().Z / zVelocity * angle;
		SetActorRotation(rotation);
	}

	//remove projectile if it stops moving
	if (isMoving && GetVelocity().X == 0.0f) {
		Stop();
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//keep projectile initially hidden
	SetActorHiddenInGame(true);
}

void AProjectile::Start(APawn *owner, bool isRight){
	//update the parent of the projectile
	parent = owner;

	//update x-component of offset to account for rotation
	offset.X = isRight ? offset.X : -offset.X;

	//correctly reset location/rotation of projectile (due to issue with spawn)
	UpdatePosition(isRight);

	//set the projectile to use move animation
	if (GetSprite()->GetFlipbook() != moveAnim) {
		GetSprite()->SetFlipbook(moveAnim);
	}

	//delegate for handling collision
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::Collide);

	//delegate to handle removal of projectile
	GetSprite()->OnFinishedPlaying.AddDynamic(this, &AProjectile::Remove);

	//update direction boolean
	moveRight = isRight;
	
	//launch projectile
	Launch(isRight);
}

void AProjectile::UpdatePosition(bool isRight) {
	//update location relative to parent
	SetActorLocation(parent->GetActorLocation() + offset);

	//update rotation relative to parent
	SetActorRotation(FRotator(angle, isRight ? 0.0f : 180.0f, 0.0f));

	//make projectile visible again
	SetActorHiddenInGame(false);
}

void AProjectile::Launch(bool isRight) {
	//set projectile delete to true
	canDelete = true;

	//toggle gravity scale on projectile
	GetCharacterMovement()->GravityScale = gravityScale;

	//compute orientation scale for projectile
	float moveScale = isRight ? 1.0f : -1.0f;

	//add velocity to projectile
	LaunchCharacter(FVector(xVelocity * moveScale, 0.0f, zVelocity), true, true);

	//update moving flag
	isMoving = true;

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(movementTimer, this, &AProjectile::Stop, travelTime, false);
}

void AProjectile::Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	//if other actor is enemy, call damage function
	if (OtherActor != this && OtherActor != parent)
	{
		//disable timed removal of projectile
		GetWorldTimerManager().ClearTimer(movementTimer);

		//disable collision box on projectile
		UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
		collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//apply damage to other actor
		if (OtherActor->IsA<AFrog>()) {
			Cast<AFrog>(OtherActor)->Damage(projectileDamage, (moveRight ? -1.0f : 1.0f));
		}
		else if (OtherActor->IsA<AEnemy>()) {
			Cast<AEnemy>(OtherActor)->Damage(1);
		}

		Stop();
	}
}

void AProjectile::Stop() {
	//stop projectile movement
	GetCharacterMovement()->StopMovementImmediately();

	//disable collision box on projectile
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//allow the collision animation to occur once
	GetSprite()->SetLooping(false);
	GetSprite()->SetFlipbook(collideAnim);
}

void AProjectile::Remove() {
	//remove projectile from level
	Destroy();
}