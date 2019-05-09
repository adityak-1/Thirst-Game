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

	// function to reset movement flags
	UFUNCTION()
		void ResetMovement();

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

	//TODO setters should use e.g. std::max(maxHealth, h) if we ever implement a heal
	//setters for currentHealth and currentWater
	UFUNCTION()
		void SetCurrentHealth(float h) { currentHealth = h; }
	UFUNCTION()
		void SetCurrentWater(float w) { currentWater = w; }

	//fully restore health and water when player overlaps well
	UFUNCTION()
		void InWell(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OutWell(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//prevent passive water loss when shaded
	UFUNCTION()
		void InShade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OutShade(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void Damage(float damageTaken, float disp);

	//delegate to handle when melee attack hits an enemy
	UFUNCTION()
		void MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//function called when health reaches 0
	UFUNCTION()
		void Die();

	//function called when health reaches 0
	UFUNCTION()
		void DieHelper();

	//function called when adding Water
	UFUNCTION()
		void AddWater(float amount);

	//function called when to respawn a new player
	UFUNCTION()
		void respawn();

	bool isKilled, bossKilled;

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

	//max health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float maxHealth;

	//max water
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float maxWater;
	
	//current health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float currentHealth;

	//current water
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float currentWater;

	//water lost per tick while not shaded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float tickWater;

	//health lost per tick while water <= 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float tickHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float respawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
		float deathDelay;

	//died screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<class UUserWidget> DiedWidget;

	//game over screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<class UUserWidget> GameOverWidget;

	//Won screen (killed the boss)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<class UUserWidget> WonWidget;

	//water cost for melee
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float meleeWater;

	//water cost for ranged
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float rangedWater;

	//water cost for dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float dashWater;

	//projectile for ranged attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		TSubclassOf<class AProjectile> projectile;

	//damage inflicted by melee attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float meleeDamage;

	//damage inflicted by ranged attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		float rangedDamage;

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
	
	//animation when player dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* deathAnim;

	//sound when player is performing melee
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundBase* meleeSound;

	//sound when player is performing ranged
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundBase* rangedSound;

private:
	bool isDash, dashAgain, isBackstep, isMelee, isRanged, isShaded, isWell, isStun;
	int shadeCount;

	FTimerHandle movementTimer;
	FTimerHandle attackTimer;
	FTimerHandle respwanTimer;
	AActor *checkPoint;
	UUserWidget* displayWidget;
	FTimerHandle deathDelayTimer;
	UAudioComponent* audioComponent;
};