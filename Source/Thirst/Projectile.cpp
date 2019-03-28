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
#include "Snake.h"
#include "Lizard.h"
#include "Scarab.h"

// Sets default values
AProjectile::AProjectile() {
	//set initial gravity of projectile to 0
	GetCharacterMovement()->GravityScale = 0.0f;

	//set projectile delete to false
	canDelete = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//keep projectile initially hidden
	SetActorHiddenInGame(true);
}

void AProjectile::Start(APawn *owner, bool isRight, bool canLaunch){
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

	if (canLaunch) {
		Launch(isRight);
	}
}

void AProjectile::UpdatePosition(bool isRight) {
	//update location relative to parent
	SetActorLocation(parent->GetActorLocation() + offset);

	//update rotation relative to parent
	SetActorRotation(FRotator(0.0f, isRight ? 0.0f : 180.0f, 0.0f));

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
	LaunchCharacter(FVector(xVelocity * moveScale, 0.0f, 0.0f), true, true);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(movementTimer, this, &AProjectile::Stop, travelTime, false);
}

void AProjectile::Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	//if other actor is enemy, call damage function
	if (OtherActor != this && OtherActor != parent && !OtherActor->IsA<AProjectile>())
	{
		//disable timed removal of projectile
		GetWorldTimerManager().ClearTimer(movementTimer);

		//disable collision box on projectile
		if (canDelete) {
			UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
			collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (OtherActor != parent) {
			//do damage to OtherActor (enemy could possibly attack another enemy)
			if (OtherActor->IsA<ASnake>()) {
				Cast<ASnake>(OtherActor)->Damage(1);
			}
			else if (OtherActor->IsA<ALizard>()) {
				Cast<ALizard>(OtherActor)->Damage(1);
			}
			else if (OtherActor->IsA<AScarab>()) {
				Cast<AScarab>(OtherActor)->Damage(1);
			}
			else if(OtherActor->IsA<AFrog>()){
				Cast<AFrog>(OtherActor)->Damage(5);
			}
		}

		if (canDelete) {
			Stop();
		}
		else {
			//TODO apparently the lizard recoils when it hits the player?
			//lizard's spear collided with player
			if (OtherActor->IsA<AFrog>() && parent->IsA<ALizard>()) {
				Cast<ALizard>(parent)->Recoil();
			}
		}
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