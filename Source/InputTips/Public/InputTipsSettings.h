// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "InputTipsSettings.generated.h"

/**
 * 
 */
UCLASS(Config = "Input", defaultconfig, meta = (DisplayName = "Input Tips Settings"))
class INPUTTIPS_API UInputTipsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	TArray<FGameplayTag> InputTags;

	int GetInputId(const FGameplayTag& InputTag) const;
	
	virtual FName GetCategoryName() const override;
};
