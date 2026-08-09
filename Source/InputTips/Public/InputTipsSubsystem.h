// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTipsDataAsset.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "InputTipsSubsystem.generated.h"

class UInputTipsDataAsset;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
struct FEnhancedActionKeyMapping;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDisplayKeyChangedDelegate, const TArray<FName>&, ChangedArray);

UCLASS()
class INPUTTIPS_API UInputTipsSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	void RebuildFastQueryMap();
	
	TMap<FName, int32> FastQueryMap;
	
	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;
	
	//UPROPERTY(BlueprintReadOnly)
	//TArray<FName> DisplayingMappableKeys;

	UPROPERTY(BlueprintReadOnly)
	TArray<UInputTipsDataAsset*> DisplayingInputTipsDataAssets;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FTagInputTipsArray> DisplayingInputTips;
	
	UPROPERTY(BlueprintAssignable)
	FOnDisplayKeyChangedDelegate OnDisplayKeysAddedEvent;

	UPROPERTY(BlueprintAssignable)
	FOnDisplayKeyChangedDelegate OnDisplayKeysRemovedEvent;

	UFUNCTION(BlueprintCallable, Category = "Input Tips")
	void AddInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset);

	UFUNCTION(BlueprintCallable, Category = "Input Tips")
	void RemoveInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset);

	UFUNCTION(BlueprintCallable, Category = "Input Tips")
	void AddSingleInputTips(FName IdentityName, FName KeyName, const FTagInputTipsArray& InTipsQuery);
	
	UFUNCTION(BlueprintCallable, Category = "Input Tips")
	void RemoveInputTipsByName(FName IdentityName);
	
	UFUNCTION(BlueprintCallable, Category = "Input Tips")
	void SetKeyAutoClear(FName KeyName, bool bAutoClear);
	
	void AddInputTips(const FName& IdentityName, const FName& KeyName, const FTagInputTipsArray& InTipsQuery, bool& bIsNew);
	void RemoveInputTips(const FName& IdentityName, TArray<FName>& EmptyTips);
	
	UFUNCTION(BlueprintPure, Category = "Input Tips")
	bool GetMappingDisplayName(FName KeyName, const UObject* TagsContainerObject, FText& DisplayName, bool& bShouldDisplay);

	// Get mappable input datas by fast mapping query in enhanced input subsystem.
	UFUNCTION(BlueprintPure, Category = "Input Tips")
	bool GetMappingKeyName(FName KeyName, FText& DisplayKeyName);

	// Base query function for input tips runtime query.
	bool QueryDisplayInputTips(FName KeyName, const UObject* TagsContainerObject, FText& DisplayInputTips, bool& bShouldDisplay);
};
