// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTipsSubsystem.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "InputTipsDataAsset.h"
#include "PlayerMappableKeySettings.h"

void UInputTipsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UEnhancedInputLocalPlayerSubsystem>();
	
	EnhancedInputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (EnhancedInputSubsystem)
	{
		RebuildFastQueryMap();
		EnhancedInputSubsystem->ControlMappingsRebuiltDelegate.AddDynamic(this, &UInputTipsSubsystem::RebuildFastQueryMap);
	}
}

void UInputTipsSubsystem::Deinitialize()
{
	FastQueryMap.Empty();
	DisplayingMappableKeys.Empty();

	EnhancedInputSubsystem = nullptr;
}

void UInputTipsSubsystem::RebuildFastQueryMap()
{
	if (!EnhancedInputSubsystem) return;

	auto Mappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
	FastQueryMap.Empty();
	for (int i = 0; i < Mappings.Num(); ++i)
	{
		FastQueryMap.Add(Mappings[i].GetPlayerMappableKeySettings()->Name, i);
	}
}

void UInputTipsSubsystem::AddInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset)
{
	DisplayingInputTipsDataAssets.Add(InputTipsDataAsset);

	TArray<FName> Names;
	InputTipsDataAsset->DisplayInputTips.GetKeys(Names);
	AddDisplayingMappableKeys(Names);
}

void UInputTipsSubsystem::RemoveInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset)
{
	DisplayingInputTipsDataAssets.Remove(InputTipsDataAsset);
	
	TArray<FName> Names;
	InputTipsDataAsset->DisplayInputTips.GetKeys(Names);
	RemoveDisplayingMappableKeys(Names);
}

void UInputTipsSubsystem::AddDisplayingMappableKeys(TArray<FName> InKeyNames)
{
	for (auto Name : InKeyNames)
	{
		DisplayingMappableKeys.AddUnique(Name);
	}
	
	OnDisplayKeysAddedEvent.Broadcast(InKeyNames);
}

void UInputTipsSubsystem::RemoveDisplayingMappableKeys(TArray<FName> InKeyNames)
{
	for (auto Name : InKeyNames)
	{
		DisplayingMappableKeys.Remove(Name);
	}

	OnDisplayKeysRemovedEvent.Broadcast(InKeyNames);
}

bool UInputTipsSubsystem::GetMappingKeyName(FName KeyName, FText& DisplayKeyName)
{
	if (!EnhancedInputSubsystem) return false;
	
	auto Mappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
	const auto Index = FastQueryMap.Find(KeyName);
	if (Index && Mappings[*Index].GetPlayerMappableKeySettings())
	{
		DisplayKeyName = Mappings[*Index].Key.GetDisplayName(false);
		return true;
	}

	return false;
}

bool UInputTipsSubsystem::GetMappingDisplayName(FName KeyName, const UObject* TagsContainerObject, FText& DisplayName, bool& bShouldDisplay)
{
	if (!EnhancedInputSubsystem) return false;

	auto Mappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
	const auto Index = FastQueryMap.Find(KeyName);
	if (Index && Mappings.IsValidIndex(*Index) && Mappings[*Index].GetPlayerMappableKeySettings())
	{
		if (QueryDisplayInputTips(KeyName, TagsContainerObject, DisplayName, bShouldDisplay))
		{
			return true;
		}
		
		DisplayName = Mappings[*Index].GetPlayerMappableKeySettings()->DisplayName;
		bShouldDisplay = true;
		return true;
	}

	return false;
}

bool UInputTipsSubsystem::QueryDisplayInputTips(const FName KeyName, const UObject* TagsContainerObject, FText& DisplayInputTips, bool& bShouldDisplay)
{
	for (const auto Itr : DisplayingInputTipsDataAssets)
	{
		for (const auto Mapping : Itr->DisplayInputTips)
		{
			if (Mapping.Key == KeyName)
			{
				return Mapping.Value.GetDisplayTips(TagsContainerObject, DisplayInputTips, bShouldDisplay);
			}
		}
	}

	DisplayInputTips = FText();
	return false;
}
