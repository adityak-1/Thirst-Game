// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Egg.generated.h"

/**
 * 
 */
UCLASS()
class THIRST_API AEgg : public APaperCharacter
{
	GENERATED_BODY()
public:

	AEgg();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Hatch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* beforeHatchAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* hatchAnim;
};
