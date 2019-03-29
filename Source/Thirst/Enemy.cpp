//Name: Aditya Kaliappan
//File: Enemy.cpp
//
//This file contains the basic functions needed for any enemy.

#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

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
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//move logic to destroy enemy once projectiles finalized
}

float AEnemy::GetPlayerDisp() {
	//get current X position
	float currX = GetCharacterMovement()->GetActorLocation().X;

	//return displacement
	return currX - enemy->GetActorLocation().X;
}

//called when an attack hits the enemy
void AEnemy::Damage(int damageTaken) {
	hitPoints -= damageTaken;
}