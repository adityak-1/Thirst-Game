// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Boss.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API ABoss : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//functions to handle boss attacks
	UFUNCTION()
		void Attack();
	UFUNCTION()
		void Thrust();
	UFUNCTION()
		void Claw();
	UFUNCTION()
		void ResetAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//time interval between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float attackInterval;

	//projectile for ranged attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AProjectile> projectile;

	//animation when boss is idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleAnim;

	//animation when boss is attacking with thrust
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* thrustAnim;

	//animation when boss is attacking with claw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* clawAnim;

private:
	FTimerHandle intervalTimer;
	UBoxComponent* attackBox;
};