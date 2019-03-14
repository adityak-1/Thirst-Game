// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Frog.generated.h"

/**
 *
 */
UCLASS()
class THIRST_API AFrog : public APaperCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFrog();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// function to handle left/right motion of player
	UFUNCTION()
		void MoveLeftRight(float val);

	// functions to handle dash motion of player
	UFUNCTION()
		void Dash();
	UFUNCTION()
		void StopDashing();

	// functions to handle backstep motion of player
	UFUNCTION()
		void Backstep();
	UFUNCTION()
		void StopBackstep();

	// functions to handle melee and ranged attacks
	UFUNCTION()
		void Melee();
	UFUNCTION()
		void ActivateMelee();
	UFUNCTION()
		void DeactivateMelee();
	UFUNCTION()
		void StopMelee();
	UFUNCTION()
		void Ranged();
	UFUNCTION()
		void ActivateRanged();
	UFUNCTION()
		void StopRanged();

	//FComponentBeginOverlapSignature OnComponentOverlap;	TODO delete this?
	UFUNCTION()
		void MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//scale gravity on player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityScale;

	//scale X-axis movement when falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float airControl;

	//initial upward velocity when jumping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float jumpZVelocity;

	//default friction for surface player walks on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float groundFriction;

	//maximum walking speed for player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float maxWalkSpeed;

	//launch velocity for dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashVel;

	//duration of dash (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float dashDuration;

	//launch velocity for backstep
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float backstepVel;

	//projectile velocity for ranged
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float rangedVel;

	//duration of backstep (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float backstepDuration;

	//duration of melee startup (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float meleeStartup;

	//duration melee hitbox is active (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float meleeActivation;

	//duration of melee ending (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float meleeEndlag;

	//duration of ranged startup (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float rangedStartup;

	//duration of ranged animation (in seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float rangedEndlag;

	//animation when player is idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* idleAnim;

	//animation when player is walking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* walkAnim;

	//animation when player is jumping (up)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* jumpAnim;

	//animation when player is falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* fallAnim;

	//animation when player is performing dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* dashAnim;

	//animation when player is performing backstep
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* backstepAnim;

	//animation when player is performing melee attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* meleeAnim;

	//animation when player is performing ranged attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* rangedAnim;

private:
	bool isDash, dashAgain, isBackstep, isMelee, isRanged;
	int meleeDamage = 2;
	int meleeWater = 1;
	int rangedDamage = 1;
	int rangedWater = 1;
	FTimerHandle movementTimer;
	FTimerHandle attackTimer;
};