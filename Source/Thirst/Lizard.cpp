//Name: Aditya Kaliappan
//File: Lizard.cpp
//
//This file contains the AI functionality for the lizard enemy.
//
//References:
//https://wiki.unrealengine.com/Function_Pointers
//http://api.unrealengine.com/INT/API/Runtime/Core/Math/FMath/RandRange/index.html

#include "Lizard.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Projectile.h"
#include "Frog.h"
#include "Engine/GameEngine.h"

// Sets default values
ALizard::ALizard() : AEnemy()
{
	//set default values for flags
	isRight = false;
	isAttack = false;
}

// Called when the game starts or when spawned
void ALizard::BeginPlay()
{
	Super::BeginPlay();

	//initialize the dagger collision box on the lizard
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALizard::Collide);
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//update animation if lizard has dagger
	if (hasDagger) {
		GetSprite()->SetFlipbook(walkDaggerAnim);
	}

	//initialize parameters for projectile spear (if it exists)
	if (hasSpear) {
		FActorSpawnParameters params;
		params.Owner = this;

		//spawn projectile
		spear = GetWorld()->SpawnActor<AProjectile>(spearProjectile, GetActorLocation(), GetActorRotation(), params);
		spear->Start(this, isRight, false);

		//update animation if lizard has spear
		GetSprite()->SetFlipbook(walkSpearAnim);
	}
}

// Called every frame
void ALizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		if (spear) {
			spear->Remove();
		}

		this->Destroy();
	}

	if (hasSpear && spear) {
		spear->UpdatePosition(isRight);
	}

	//get X component of lizard velocity
	float xVel = GetVelocity().X;

	//lizard can attack
	if (CanAttack()) {
		Attack();
	}
	//lizard not near player to attack
	else if (!isAttack) {
		//compute rotation for lizard to face correct direction
		float yawAngle = (xVel > 0.0f) ? 180.0f : ((xVel < 0.0f) ? 0.0f :
			GetControlRotation().Yaw);

		//rotate player based on direction of motion
		SetActorRotation(FRotator(0.0f, yawAngle, 0.0f));

		//select correct walk animation
		UPaperFlipbook* currWalk = hasSpear ? walkSpearAnim : (hasDagger ? walkDaggerAnim : walkAnim);

		//update animation to walk if not already as such
		if (GetSprite()->GetFlipbook() != currWalk) {
			GetSprite()->SetFlipbook(currWalk);
		}

		Walk();
	}
}

void ALizard::Walk() {
	//move lizard slightly in current frame
	AddMovementInput(FVector(speedScale, 0.0f, 0.0f), (isRight ? 1.0f : -1.0f));

	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//get displacement from starting point
	float disp = currX - center.X;

	//bound lizard movement within radius
	if ((isRight && disp > moveRadius) || (!isRight && -disp > moveRadius)) {
		//stop current movement
		GetCharacterMovement()->StopMovementImmediately();

		//flip lizard direction
		isRight = !isRight;
	}
}

bool ALizard::CanAttack() {
	//get displacement to player
	float disp = GetPlayerDisp();

	//return whether lizard can perform attack
	return !isAttack && abs(disp) <= visionDist && (isRight == (disp < 0));
}

void ALizard::Attack() {
	//update attack flag to true
	isAttack = true;

	//update lizard to face player (if incorrect)
	SetActorRotation(FRotator(0.0f, (isRight ? 180.0f : 0.0f), 0.0f));

	//stop lizard walk movement
	GetCharacterMovement()->StopMovementImmediately();

	//select attack to make
	int randInt = FMath::RandRange(0, 1);

	FunctionPtr ptr = (randInt == 0) ? &ALizard::Spit :
		(hasSpear ? &ALizard::Spear : 
		(hasDagger ? &ALizard::Dagger : &ALizard::Spit));

	//wait for some time and start attack
	GetWorld()->GetTimerManager().SetTimer(startTimer, this,
		ptr, attackDelay, false);
}

void ALizard::Spit() {
	//select correct idle animation
	UPaperFlipbook* currIdle = hasSpear ? idleSpearAnim : (hasDagger ? idleDaggerAnim : idleAnim);
	GetSprite()->SetFlipbook(currIdle);

	//initialize parameters for projectile spawn
	FActorSpawnParameters params;
	params.Owner = this;

	//spawn projectile
	AProjectile* waterBall = GetWorld()->SpawnActor<AProjectile>(projectile, GetActorLocation(), GetActorRotation(), params);

	//allow the projectile to move
	waterBall->Start(this, isRight);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, resetDelay, false);
}

void ALizard::Spear() {
	//select dash or throw
	int randInt = FMath::RandRange(0, 1);

	//dash with spear
	if (randInt == 0) {
		Dash();
	}
	//throw spear
	else {
		GetSprite()->SetLooping(false);
		GetSprite()->SetFlipbook(throwAnim);

		hasSpear = false;
		spear->Launch(isRight);
	}

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, (randInt == 0 ? dashTime : resetDelay), false);
}

void ALizard::Dagger() {
	//enable collision box for dagger
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//dash with dagger
	Dash();

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, dashTime, false);
}

void ALizard::Dash() {
	//select correct dash animation
	UPaperFlipbook* currDash = hasSpear ? dashSpearAnim : dashDaggerAnim;

	//update animation to dash
	GetSprite()->SetFlipbook(currDash);

	//begin movement for dash
	LaunchCharacter(FVector(dashVel * (isRight ? 1.0f : -1.0f), 0.0f, 0.0f), true, true);
}

void ALizard::Recoil() {
	//select correct recoil animation
	UPaperFlipbook* currRecoil = hasSpear ? recoilSpearAnim : recoilDaggerAnim;

	//update animation to dash
	GetSprite()->SetFlipbook(currRecoil);

	//begin movement for recoil
	LaunchCharacter(FVector(recoilVel * (isRight ? -1.0f : 1.0f), 0.0f, 0.0f), true, true);

	//wait for some time, end animation
	GetWorldTimerManager().SetTimer(resetTimer, this,
		&ALizard::ResetAttack, resetDelay, false);
}

void ALizard::ResetAttack() {
	//disable collision box for dagger
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//reset attack flag to false
	isAttack = false;

	//reset animation settings
	GetSprite()->SetLooping(true);
	GetSprite()->Play();
}

void ALizard::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor == enemy && OtherComp->GetName() != "MeleeCollision") {
		Cast<AFrog>(OtherActor)->Damage(5, GetPlayerDisp());
	}

	if (OtherActor != this) {
		//check whether lizard is attacking
		if (isAttack) {
			//disable timed ending of attack
			GetWorldTimerManager().ClearTimer(resetTimer);

			//allow lizard to recoil
			Recoil();
		}
		//lizard is not attacking
		else {
			//flip direction of lizard
			isRight = !isRight;
		}
	}
}