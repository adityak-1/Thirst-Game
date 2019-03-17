// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Lizard.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API ALizard : public APaperCharacter
{
	GENERATED_BODY()

public:
	//function pointer typedef
	typedef void (ALizard::*FunctionPtr)(void);

	// Sets default values for this character's properties
	ALizard();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// function to get displacement to player
	UFUNCTION()
		float GetPlayerDisp();

	// function to handle lizard walk
	UFUNCTION()
		void Walk();

	//functions to handle lizard attack
	UFUNCTION()
		bool CanAttack();
	UFUNCTION()
		void Attack();
	UFUNCTION()
		void Spit();
	UFUNCTION()
		void Spear();
	UFUNCTION()
		void Dagger();
	UFUNCTION()
		void ResetAttack();

	//function for lizard to take damage
	UFUNCTION()
		void Damage(int damageTaken);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale for lizard speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float speedScale;

	//radius that lizard can travel within
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float moveRadius;

	//distance that lizard can see ahead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float visionDist;

	//projectile for spit attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AProjectile> projectile;

	//projectile for spear attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AProjectile> spearProjectile;

	//flag for whether lizard has dagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool hasDagger;
	
	//flag for whether lizard has spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool hasSpear;

	//delay before lizard attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float attackDelay;

	//delay to reset attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float resetDelay;

	//animation when player is walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkAnim;

	//health of lizard
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int hitPoints;

private:
	FVector center;
	APawn* enemy;
	AProjectile* spear;
	bool isRight;
	bool isAttack;
	FTimerHandle startTimer;
	FTimerHandle resetTimer;
};
