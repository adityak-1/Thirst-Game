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

// Sets default values
AProjectile::AProjectile() {
	//set initial gravity of projectile to 0
	GetCharacterMovement()->GravityScale = 0.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//keep projectile initially hidden
	SetActorHiddenInGame(true);
}

void AProjectile::Start(APawn *parent){
	//correctly reset location of projectile (due to issue with spawn)
	SetActorLocation(parent->GetActorLocation() + offset);

	//set the projectile to use move animation
	GetSprite()->SetFlipbook(moveAnim);

	//make projectile visible again
	SetActorHiddenInGame(false);

	//delegate for handling collision
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::Collide);

	//delegate to handle removal of projectile
	GetSprite()->OnFinishedPlaying.AddDynamic(this, &AProjectile::Remove);

	//toggle gravity scale on projectile
	GetCharacterMovement()->GravityScale = gravityScale;

	//add velocity to projectile
	LaunchCharacter(FVector(xVel + parent->GetVelocity().X, 0.0f, 0.0f), true, true);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(movementTimer, this, &AProjectile::Stop, travelTime, false);
}

void AProjectile::Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	//if other actor is enemy, call damage function
	if (OtherActor != this)
	{
		//disable timed removal of projectile
		GetWorldTimerManager().ClearTimer(movementTimer);

		//disable collision box on projectile
		UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
		collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//get player
		AActor *player = GetWorld()->GetFirstPlayerController()->GetPawn();

		//check if damage should be done on collision
		if ((OtherActor->GetName() == player->GetName()) != canDamagePlayer) {
			//do damage to OtherActor
		}

		Stop();
	}
}

void AProjectile::Stop() {
	//stop projectile movement
	GetCharacterMovement()->StopMovementImmediately();

	//allow the collision animation to occur once
	GetSprite()->SetLooping(false);
	GetSprite()->SetFlipbook(collideAnim);
}

void AProjectile::Remove() {
	//remove projectile from level
	Destroy();
}