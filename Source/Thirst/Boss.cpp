//Name: Aditya Kaliappan
//File: Boss.cpp
//
//This file contains the AI functionality for the boss.
//
//References:
//https://answers.unrealengine.com/questions/4912/bug-spawned-actor-works-differently-from-the-bluep.html
//https://answers.unrealengine.com/questions/264977/how-to-use-fpthreadscriticalsection-make-data-thre.html
//https://wiki.unrealengine.com/String_Conversions:_FString_to_FName,_FString_to_Int32,_Float_to_FString
//http://api.unrealengine.com/INT/API/Runtime/Core/Math/FMath/RandRange/index.html

#include "Boss.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Frog.h"
#include "Engine.h"

// Sets default values
ABoss::ABoss() : AEnemy()
{
	//set gravity to 0 so that boss stays in place
	GetCharacterMovement()->GravityScale = 0;
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

	//initialize array that maintains scarabs
	for (int i = 0; i < scarabRelLocation.Num(); i++) {
		availablePos.Add(i);
	}

	//spawn a scarab in a fixed interval
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this,
		&ABoss::Spawn, spawnInterval, true);
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

void ABoss::Spawn() {
	//lock this section so that array can be modified
	FScopeLock ScopeLock(&m_mutex);

	//spawn another scarab if max capacity has not been reached
	if (availablePos.Num() > 0) {
		//generate random index in array
		int randInt = FMath::RandRange(0, availablePos.Num() - 1);

		//get position at specified index
		int spawnPos = availablePos[randInt];

		//initialize parameters to spawn scarab
		FActorSpawnParameters params;
		params.Owner = this;
		params.Name = FName(*FString::FromInt(spawnPos));

		//remove selected position from array
		availablePos.RemoveAt(randInt);

		//spawn new scarab
		GetWorld()->SpawnActor<AEnemy>(scarab, GetActorLocation() + scarabRelLocation[spawnPos],
			GetActorRotation(), params)->SpawnDefaultController();
	}
}

void ABoss::AddSpawnPoint(int pos) {
	//lock this section so that array can be modified
	FScopeLock ScopeLock(&m_mutex);

	//add position to array of available positions
	availablePos.AddUnique(pos);
}