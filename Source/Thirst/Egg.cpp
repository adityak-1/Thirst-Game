// Fill out your copyright notice in the Description page of Project Settings.

#include "Egg.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "PaperFlipbookComponent.h"
#include "Engine/World.h"

AEgg::AEgg()
{

}

void AEgg::BeginPlay() {
	Super::BeginPlay();
	GetSprite()->SetFlipbook(beforeHatchAnim);
}

void AEgg::Hatch() {
	GetSprite()->SetLooping(false);
	GetSprite()->Play();
	GetSprite()->SetFlipbook(hatchAnim);
}

