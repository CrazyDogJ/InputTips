// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTipsLibrary.h"

#include "InputTipsSettings.h"

int UInputTipsLibrary::GetInputIdByTag(FGameplayTag InputTag)
{
	return GetDefault<UInputTipsSettings>()->GetInputId(InputTag);
}
