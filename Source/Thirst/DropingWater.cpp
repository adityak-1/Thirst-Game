// Fill out your copyright notice in the Description page of Project Settings.

#include "DropingWater.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Frog.h"
#include "Boss.h"

void ADropingWater::BeginPlay()
{
	Super::BeginPlay();

	//set enemy to target player
	enemy = GetWorld()->GetFirstPlayerController()->GetPawn();

	//get the collision box on the scarab
	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collision")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADropingWater::Collide);
}

void ADropingWater::Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//check if player was hit
	if (OtherActor == enemy) {
		//player add CurrentWater
		((AFrog*)OtherActor)->AddWater(healAmount);
		this->Destroy();
	}
}