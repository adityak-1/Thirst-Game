// Fill out your copyright notice in the Description page of Project Settings.

#include "FrogAditya.h"

// Sets default values
AFrogAditya::AFrogAditya()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrogAditya::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFrogAditya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFrogAditya::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

