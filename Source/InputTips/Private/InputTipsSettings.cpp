// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTipsSettings.h"

int UInputTipsSettings::GetInputId(const FGameplayTag& InputTag) const
{
	return InputTags.Find(InputTag);
}

FName UInputTipsSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
