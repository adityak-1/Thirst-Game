//Name: Aditya Kaliappan
//File: Enemy.cpp
//
//This file contains the basic functions needed for any enemy.

#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Scarab.h"
#include "Snake.h"
#include "Lizard.h"
#include "Boss.h"
#include "Components/AudioComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	//set this character to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//get the position of enemy as movement center
	center = GetCharacterMovement()->GetActorLocation();

	//set enemy to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//initialize audio component for enemy sound effects
	audioComponent = Cast<UAudioComponent>(GetDefaultSubobjectByName(TEXT("Audio")));
	audioComponent->bAutoActivate = false;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//if hit points go to or below 0, destroy this actor
	if (hitPoints <= 0) {
		//check if enemy is a scarab
		if(this->IsA<AScarab>()||this->IsA<ASnake>()||this->IsA<ALizard>()||this->IsA<ABoss>())
			Death();
		else
			this->Destroy();
	}
}

void AEnemy::Death() {
	
}

float AEnemy::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//return displacement
	return currX - enemy->GetActorLocation().X;
}

//called when an attack hits the enemy
void AEnemy::Damage(int damageTaken) {
	//play hit sound
	audioComponent->SetSound(hitSound);
	audioComponent->Play();

	hitPoints -= damageTaken;
}