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
//https://answers.unrealengine.com/questions/165678/using-settimer-on-a-function-with-parameters.html

#include "Boss.h"
#include "TimerManager.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Frog.h"
#include "Engine.h"
#include "CustomGameInstance.h"

// Sets default values
ABoss::ABoss() : AEnemy()
{
	//set gravity to 0 so that boss stays in place
	GetCharacterMovement()->GravityScale = 0;

	numScarabs = 0;
	isKilled = false;
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

		//initialize parameters to spawn egg
		FActorSpawnParameters params;
		params.Owner = this;
		FVector location = scarabRelLocation[i] + FVector(0.0f, 1.0f, 0.0f);

		//spawn new egg
		eggs.Add(GetWorld()->SpawnActor<AEgg>(egg, GetActorLocation() + location, 
			GetActorRotation(), params));
	}

	//spawn a scarab in a fixed interval
	GetWorld()->GetTimerManager().SetTimer(spawnTimer, this,
		&ABoss::Spawn, spawnInterval, true);
}

void ABoss::Death() {
	//change hitpoints to avoid call death multi times
	hitPoints = 1000;

	//set isKilled to true
	isKilled = true;

	//set player is stun and killed
	((AFrog*)enemy)->isKilled = true;
	((AFrog*)enemy)->bossKilled = true;

	//change animation and walking method
	GetCharacterMovement()->StopMovementImmediately();
	GetSprite()->SetFlipbook(deathAnim);

	//turn off attack hitbox
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ABoss::DeathHelper, deathDelay, false);
}

void ABoss::DeathHelper() {
	this->TeleportTo(FVector(GetActorLocation().X + finalWaterAdjustment, GetActorLocation().Y, GetActorLocation().Z), FRotator(0.0f, 0.0f, 0.0f), false, false);
	GetCharacterMovement()->StopMovementImmediately();
	GetSprite()->SetFlipbook(finalWaterAnim);

	//this->Destroy();
	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ABoss::DeathHelperHelper, finalWaterDelay, false);
}

void ABoss::DeathHelperHelper() {
	GetSprite()->SetLooping(true);
	GetSprite()->SetFlipbook(finalWaterLoopAnim);

	//set game done flag to start fade out
	Cast<UCustomGameInstance>(GetGameInstance())->gameDone = true;

	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ABoss::Won, wonDelay, false);
}

void ABoss::Won() {
	if (((AFrog *)enemy)->WonWidget) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		UUserWidget* displayWidget = CreateWidget<UUserWidget>(GetWorld(), ((AFrog *)enemy)->WonWidget);

		// now you can use the widget directly since you have a referance for it.
		// Extra check to  make sure the pointer holds the widget.
		if (displayWidget)
		{
			//let add it to the view port
			displayWidget->AddToViewport();
		}
	}
}

void ABoss::Attack() {
	if (!isKilled) {
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
	if (!isKilled) {
		if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
			//disable attack collision box
			attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			Cast<AFrog>(OtherActor)->Damage(20, 1.0f);
		}
	}
}

void ABoss::Spawn() {
	if (!isKilled) {
		//lock this section so that array can be modified
		FScopeLock ScopeLock(&m_mutex);

		//spawn another scarab if max capacity has not been reached
		if (numScarabs < 2 && availablePos.Num() > 0) {
			numScarabs++;

			//generate random index in array
			int randInt = FMath::RandRange(0, availablePos.Num() - 1);

			//get position at specified index
			int spawnPos = availablePos[randInt];

			//remove selected position from array
			availablePos.RemoveAt(randInt);

			//set corresponding egg to hatch
			if (eggs[spawnPos]->IsValidLowLevel())
				eggs[spawnPos]->Hatch();

			scarabFunc.BindUFunction(this, FName("AddScarab"), spawnPos);

			//spawn a scarab with delay due to egg hatch
			GetWorld()->GetTimerManager().SetTimer(eggTimer,
				scarabFunc, hatchDelay, false);
		}
	}
}

void ABoss::AddSpawnPoint(int pos) {
	//lock this section so that array can be modified
	FScopeLock ScopeLock(&m_mutex);

	numScarabs--;

	//add position to array of available positions
	//availablePos.AddUnique(pos);
}

void ABoss::AddScarab(int pos) {
	if (!isKilled) {
		//initialize parameters to spawn scarab
		FActorSpawnParameters params;
		params.Owner = this;
		params.Name = FName(*FString::FromInt(pos));

		//spawn new scarab
		GetWorld()->SpawnActor<AEnemy>(scarab, GetActorLocation() + scarabRelLocation[pos],
			GetActorRotation(), params)->SpawnDefaultController();
	}
}