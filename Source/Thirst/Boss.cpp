//Name: Aditya Kaliappan
//File: Boss.cpp
//
//This file contains the AI functionality for the boss.

#include "Boss.h"
#include "Engine.h"
#include "TimerManager.h"

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
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABoss::Attack() {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attack"));
}