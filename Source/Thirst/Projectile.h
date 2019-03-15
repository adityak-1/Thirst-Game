// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Projectile.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API AProjectile : public APaperCharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AProjectile();

	//function to handle start of projectile motion
	UFUNCTION()
		void Start(APawn *parent);

	//function to handle collision with objects
	UFUNCTION()
		void Collide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//function to handle stopping of projectile motion
	UFUNCTION()
		void Stop();

	//function to remove projectile from level
	UFUNCTION()
		void Remove();

	//offset for launch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FVector offset;

	//x-velocity of projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float xVel;

	//toggle gravity scale on projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float gravityScale;

	//time projectile can travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float travelTime;

	//can projectile damage player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		bool canDamagePlayer;

	//animation when projectile is moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* moveAnim;

	//animation when projectile collides
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* collideAnim;

private:
	FTimerHandle movementTimer;
};
