// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "PaperCharacter.h"
#include "Lizard.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API ALizard : public AEnemy
{
	GENERATED_BODY()

public:
	//function pointer typedef
	typedef void (ALizard::*FunctionPtr)(void);

	// Sets default values for this character's properties
	ALizard();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
		void Dash();
	UFUNCTION()
		void Recoil();
	UFUNCTION()
		void ResetAttack();
	UFUNCTION()
		void Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

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

	//lizard dash velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashVel;

	//lizard recoil velocity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float recoilVel;

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

	//time for lizard dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashTime;

	//animation when lizard is idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleAnim;

	//animation when lizard is idle with dagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleDaggerAnim;

	//animation when lizard is idle with spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleSpearAnim;

	//animation when lizard is walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkAnim;

	//animation when lizard is walking with dagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkDaggerAnim;

	//animation when lizard is walking with spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkSpearAnim;

	//animation when lizard is dashing with dagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* dashDaggerAnim;

	//animation when lizard is dashing with spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* dashSpearAnim;

	//animation when lizard is recoiling with dagger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* recoilDaggerAnim;

	//animation when lizard is recoiling with spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* recoilSpearAnim;

	//animation when lizard throws spear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* throwAnim;

private:
	AProjectile* spear;
	bool isAttack;
	FTimerHandle startTimer;
	FTimerHandle resetTimer;
};