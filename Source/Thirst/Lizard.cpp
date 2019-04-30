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
	isKilled = false;
}

// Called when the game starts or when spawned
void ALizard::BeginPlay()
{
	Super::BeginPlay();

	//initialize collision box on lizard
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Boundary")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALizard::Collide);

	//initialize the dagger collision box on the lizard
	collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALizard::WeaponCollide);
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//delegate to handle throw animation
	GetSprite()->OnFinishedPlaying.AddDynamic(this, &ALizard::ThrowSpear);
}

// Called every frame
void ALizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isKilled) {
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
}

void ALizard::Death() {
	//change hitpoints to avoid call death multi times
	hitPoints = 1000;

	//set isKilled to true
	isKilled = true;

	//change animation and walking method
	GetCharacterMovement()->StopMovementImmediately();
	GetSprite()->SetFlipbook(deathAnim);

	//turn off attack hitbox
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ALizard::DeathHelper, 1.15, false);
}

void ALizard::DeathHelper() {
	GetSprite()->SetFlipbook(deathStillAnim);
	GetWorld()->GetTimerManager().SetTimer(deathDelayTimer, this,
		&ALizard::DeathHelperHelper, deathDelay, false);
}

void ALizard::DeathHelperHelper() {
	this->Destroy();
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

	//select correct idle animation
	UPaperFlipbook* currIdle = hasSpear ? idleSpearAnim : (hasDagger ? idleDaggerAnim : idleAnim);
	GetSprite()->SetFlipbook(currIdle);

	//select attack to make
	float disp = GetPlayerDisp();

	FunctionPtr ptr = hasSpear ? &ALizard::Spear :
		&ALizard::Dagger;

	//wait for some time and start attack
	GetWorld()->GetTimerManager().SetTimer(startTimer, this,
		ptr, attackDelay, false);
}

void ALizard::SpitStart() {
	if (!isKilled) {
		GetSprite()->SetLooping(false);
		GetSprite()->SetFlipbook(startAcidAnim);

		//wait for some time, finish acid attack
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::SpitFinish, acidTime, false);
	}
}

void ALizard::SpitFinish() {	//initialize parameters for projectile spawn
	if (!isKilled) {
		GetSprite()->SetFlipbook(endAcidAnim);

		FActorSpawnParameters params;
		params.Owner = this;

		//spawn projectile
		AProjectile* acidBall = GetWorld()->SpawnActor<AProjectile>(projectile, GetActorLocation(), GetActorRotation(), params);

		//allow the projectile to move
		if(acidBall->IsValidLowLevel())
			acidBall->Start(this, isRight);

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::ResetAttack, resetDelay, false);
	}
}

void ALizard::Spear() {

	if (!isKilled) {
		//select dash or throw
		int randInt = FMath::RandRange(0, 1);

		float disp = GetPlayerDisp();

		//dash with spear
		if (disp < longRange) {
			DashStart();
		}
		//throw spear
		else {
			GetSprite()->SetLooping(false);
			GetSprite()->SetFlipbook(startThrowAnim);

			hasSpear = false;
		}
	}
}

void ALizard::ThrowSpear() {
	//spawn projectile if starting throw
	if (GetSprite()->GetFlipbook() == startThrowAnim) {
		//initialize parameters for projectile spawn
		FActorSpawnParameters params;
		params.Owner = this;

		//spawn spear
		AProjectile* spear = GetWorld()->SpawnActor<AProjectile>(spearProjectile, GetActorLocation(), GetActorRotation(), params);

		//allow the projectile to move
		if(spear->IsValidLowLevel())
			spear->Start(this, isRight);

		GetSprite()->SetFlipbook(endThrowAnim);
		GetSprite()->Play();

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::ResetAttack, throwTime, false);
	}
}

void ALizard::Dagger() {

	if (!isKilled) {
		//select attack to make
		float disp = GetPlayerDisp();

		FunctionPtr ptr = (disp > longRange) ? &ALizard::DashStart :
			(disp > midRange) ? &ALizard::SpitStart :
			&ALizard::StabStart;

		//wait for some time and start attack
		GetWorld()->GetTimerManager().SetTimer(startTimer, this,
			ptr, attackDelay, false);
	}
}

void ALizard::StabStart() {
	if (!isKilled) {
		//update animation to stab
		GetSprite()->SetFlipbook(stabAnim);

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::StabFinish, stabTime, false);
	}
}

void ALizard::StabFinish() {
	if (!isKilled) {
		UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
		collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::ResetAttack, resetDelay, false);
	}
}

void ALizard::DashStart() {
	if (!isKilled) {
		//select correct dash animation
		UPaperFlipbook* currDash = hasSpear ? dashSpearAnim : dashDaggerAnim;

		//update animation to dash
		GetSprite()->SetFlipbook(currDash);

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::DashFinish, dashTime, false);
	}
}

void ALizard::DashFinish() {
	if (!isKilled) {
		//enable collision box for weapon
		UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
		collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		//begin movement for dash
		LaunchCharacter(FVector(dashVel * (isRight ? 1.0f : -1.0f), 0.0f, 0.0f), true, true);

		//wait for some time, end animation
		GetWorldTimerManager().SetTimer(resetTimer, this,
			&ALizard::ResetAttack, resetDelay, false);
	}
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
	if (!isKilled) {
		//disable collision box for weapon
		UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
		collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//reset attack flag to false
		isAttack = false;

		//select correct idle animation
		UPaperFlipbook* currIdle = hasSpear ? idleSpearAnim : (hasDagger ? idleDaggerAnim : idleAnim);

		//reset animation settings
		GetSprite()->SetFlipbook(currIdle);
		GetSprite()->SetLooping(true);
		GetSprite()->Play();
	}
}

void ALizard::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if this object is killed
	if (!isKilled) {
		//check if player was hit
		if (OtherActor == enemy && OtherComp->GetName() == "CollisionCylinder") {
			Cast<AFrog>(OtherActor)->Damage(5, GetPlayerDisp());

			//disable timed ending of attack
			GetWorldTimerManager().ClearTimer(resetTimer);

			ResetAttack();
		}
		else if (OtherActor != this && !OtherActor->IsA<AProjectile>()) {
			//flip direction of lizard
			isRight = !isRight;
		}
	}
}

void ALizard::WeaponCollide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor->IsA<AFrog>() && OtherComp->GetName() == "CollisionCylinder") {
		Cast<AFrog>(OtherActor)->Damage(10, GetPlayerDisp());
	}

	if (OtherActor != this) {
		//disable timed ending of attack
		//GetWorldTimerManager().ClearTimer(resetTimer);

		//allow lizard to recoil
		//Recoil();
		//ResetAttack();
	}
}