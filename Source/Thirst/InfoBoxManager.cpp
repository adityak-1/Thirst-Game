// Fill out your copyright notice in the Description page of Project Settings.

#include "InfoBoxManager.h"
#include "UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine.h"

// Sets default values
AInfoBoxManager::AInfoBoxManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInfoBoxManager::BeginPlay()
{
	Super::BeginPlay();

	//get reference to player
	player = GetWorld()->GetFirstPlayerController()->GetPawn();

	CreateWidget<UUserWidget>(GetWorld(), startBox)->AddToViewport();
}

// Called every frame
void AInfoBoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%f"), GetPlayerDisp()));
}

float AInfoBoxManager::GetPlayerDisp() {
	//return displacement
	return player->GetActorLocation().X - GetActorLocation().X;
}