// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputTipsDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FTagInputTips
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagQuery Query;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDisplay = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText DisplayTips;
};

USTRUCT(BlueprintType)
struct FTagInputTipsArray
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTagInputTips> TipsTagQuery;

	bool GetDisplayTips(const UObject* TagAssetObject, FText& DisplayTips, bool& bDisplay) const;
};

UCLASS(BlueprintType)
class INPUTTIPS_API UInputTipsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input Tips")
	TMap<FName, FTagInputTipsArray> DisplayInputTips;
};
