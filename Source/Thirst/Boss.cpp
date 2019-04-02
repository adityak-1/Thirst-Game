//Name: Aditya Kaliappan
//File: Boss.cpp
//
//This file contains the AI functionality for the boss.

#include "Boss.h"
#include "Engine.h"

// Sets default values
ABoss::ABoss() : AEnemy()
{
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}