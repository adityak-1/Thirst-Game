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

	//starting info box has input
	widgets.Add(NULL);

	//declare variables
	FString boxName;
	UBoxComponent* collisionBox;

	//initialize array that maintains info box widgets
	for (int i = 1; i < infoBoxes.Num(); i++) {
		//get corresponding collision box
		boxName = "Sign" + FString::FromInt(i);
		collisionBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(*boxName));

		//create overlap trigger to display box
		collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AInfoBoxManager::DisplayBox);

		//info box has input and only appears once in game
		if (infoBoxInput[i]) {
			widgets.Add(NULL);
		}
		else {
			//spawn widget ahead of time
			UUserWidget *widgetPtr = CreateWidget<UUserWidget>(GetWorld(), infoBoxes[i]);
			widgetPtr->AddToViewport();
			widgetPtr->SetVisibility(ESlateVisibility::Collapsed);
			widgets.Add(widgetPtr);

			//create end overlap trigger to hide box
			collisionBox->OnComponentEndOverlap.AddDynamic(this, &AInfoBoxManager::HideBox);
		}
	}

	//display initial spawn info box
	CreateWidget<UUserWidget>(GetWorld(), infoBoxes[0])->AddToViewport();
}

void AInfoBoxManager::DisplayBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor == player && OtherComp->GetName() == "CollisionCylinder") {
		//get collision box name
		FString boxName = OverlappedComp->GetName();

		//get index of widget from collision box
		int index = FCString::Atoi(*boxName.RightChop(4));

		//info box has input
		if (infoBoxInput[index]) {
			//info box should only appear once, so disable collision box
			OverlappedComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//spawn info box in game
			CreateWidget<UUserWidget>(GetWorld(), infoBoxes[index])->AddToViewport();
		}
		else {
			//set visibility since widget already exists
			widgets[index]->SetVisibility(ESlateVisibility::Visible);
		}

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "show" + FString::FromInt(index));
	}
}

void AInfoBoxManager::HideBox(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor == player && OtherComp->GetName() == "CollisionCylinder") {
		//get collision box name
		FString boxName = OverlappedComp->GetName();

		//get index of widget from collision box
		int index = FCString::Atoi(*boxName.RightChop(4));

		//hide info box widget
		widgets[index]->SetVisibility(ESlateVisibility::Collapsed);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "hide" + FString::FromInt(index));
	}
}