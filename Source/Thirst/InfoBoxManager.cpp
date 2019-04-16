// Fill out your copyright notice in the Description page of Project Settings.

#include "InfoBoxManager.h"
#include "UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/BoxComponent.h"
#include "Engine.h"

// Sets default values
AInfoBoxManager::AInfoBoxManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AInfoBoxManager::BeginPlay()
{
	Super::BeginPlay();

	//get reference to player
	player = GetWorld()->GetFirstPlayerController()->GetPawn();

	UBoxComponent* collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Sign1")));
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AInfoBoxManager::DisplayBox);

	CreateWidget<UUserWidget>(GetWorld(), startBox)->AddToViewport();
}

void AInfoBoxManager::DisplayBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == player && OtherComp->GetName() == "CollisionCylinder") {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "display sign");
	}
}