// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputTipsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INPUTTIPS_API UInputTipsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Input")
	static int GetInputIdByTag(FGameplayTag InputTag);
};
