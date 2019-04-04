// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

public:
	// constructor for Enemy
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Death();

	// function to get displacement to player
	UFUNCTION()
		float GetPlayerDisp();

	//function for enemy to take damage
	UFUNCTION()
		void Damage(int damageTaken);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//health of enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int hitPoints;

	//starting position of enemy
	FVector center;

	//character that enemy is attacking
	APawn* enemy;

	//flag for if enemy is facing right
	bool isRight;
};