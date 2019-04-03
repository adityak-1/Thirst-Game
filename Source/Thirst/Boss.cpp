//Name: Aditya Kaliappan
//File: Boss.cpp
//
//This file contains the AI functionality for the boss.

#include "Boss.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Frog.h"

// Sets default values
ABoss::ABoss() : AEnemy()
{
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	//make an attack in a fixed interval
	GetWorld()->GetTimerManager().SetTimer(intervalTimer, this,
		&ABoss::Attack, attackInterval, true);

	//attach attack collision box to socket on sprite
	attackBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	attackBox->AttachToComponent(GetSprite(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "collisionSocket");
	attackBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss::Collide);
	attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//activate collision capsule for boss boundary
	UCapsuleComponent* capsule = GetCapsuleComponent();
	capsule->OnComponentBeginOverlap.AddDynamic(this, &ABoss::Collide);

	//delegate to handle reset of attack animation
	GetSprite()->OnFinishedPlaying.AddDynamic(this, &ABoss::ResetAttack);
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoss::Attack() {
	//select thrust or claw
	int randInt = FMath::RandRange(0, 1);

	//enable collision box
	attackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//call selected attack function
	if (randInt == 0) {
		Thrust();
	}
	else {
		Claw();
	}
}

void ABoss::Thrust() {
	//set animation to thrust
	GetSprite()->SetLooping(false);
	GetSprite()->SetFlipbook(thrustAnim);
}

void ABoss::Claw() {
	//set animation to claw
	GetSprite()->SetLooping(false);
	GetSprite()->SetFlipbook(clawAnim);
}

void ABoss::ResetAttack() {
	//disable collision box
	attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//reset animation back to idle
	GetSprite()->SetFlipbook(idleAnim);
	GetSprite()->SetLooping(true);
	GetSprite()->Play();
}

void ABoss::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
		//disable attack collision box
		attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Cast<AFrog>(OtherActor)->Damage(5, 1.0f);
	}
}