// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Egg.h"
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
	
	//functions to handle boss attacks
	UFUNCTION()
		void Attack();
	UFUNCTION()
		void Thrust();
	UFUNCTION()
		void Claw();
	UFUNCTION()
		void ResetAttack();
	UFUNCTION()
		void Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//function to handle scarab spawn
	UFUNCTION()
		void Spawn();
	UFUNCTION()
		void AddScarab(int pos);

	UFUNCTION()
		void AddSpawnPoint(int pos);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//time interval between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float attackInterval;

	//time interval between scarab spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float spawnInterval;

	//delay between egg hatch and scarab spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float hatchDelay;

	//relative locations for scarab spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		TArray<FVector> scarabRelLocation;

	//scarab to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AEnemy> scarab;

	//egg to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AEgg> egg;

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
	FTimerHandle spawnTimer;
	FTimerHandle eggTimer;
	FTimerDelegate scarabFunc;
	UBoxComponent* attackBox;
	TArray<int> availablePos;
	TArray<AEgg*> eggs;
	FCriticalSection m_mutex;
	int numScarabs;
};