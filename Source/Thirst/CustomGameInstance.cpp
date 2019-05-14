// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomGameInstance.h"

UCustomGameInstance::UCustomGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	spawnLocation = FVector::ZeroVector;
	showStartBox = true;
	gameDone = false;
}