// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.h"
#include "DropingWater.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API ADropingWater : public AEnemy
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void Collide(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale for scarab speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
		float healAmount;

	//character that enemy is attacking
	APawn* enemy;
};
