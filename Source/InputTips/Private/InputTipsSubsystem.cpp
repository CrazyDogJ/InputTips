// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTipsSubsystem.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableKeySettings.h"

void UInputTipsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
	// 修复：子系统关闭时解除绑定并释放当前显示的提示资产。
	if (EnhancedInputSubsystem)
	{
		EnhancedInputSubsystem->ControlMappingsRebuiltDelegate.RemoveDynamic(this, &UInputTipsSubsystem::RebuildFastQueryMap);
	}
	DisplayingInputTipsDataAssets.Empty();
	DisplayingInputTips.Empty();

	EnhancedInputSubsystem = nullptr;
	Super::Deinitialize();
}

void UInputTipsSubsystem::RebuildFastQueryMap()
{
	if (!EnhancedInputSubsystem) return;

	auto Mappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
	FastQueryMap.Empty();
	for (int i = 0; i < Mappings.Num(); ++i)
	{
		// 修复：跳过没有玩家映射设置的动作。
		if (const auto Settings = Mappings[i].GetPlayerMappableKeySettings())
		{
			FastQueryMap.Add(Settings->Name, i);
		}
	}
}

void UInputTipsSubsystem::AddInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset)
{
	if (!InputTipsDataAsset)
	{
		return;
	}
	
	DisplayingInputTipsDataAssets.Add(InputTipsDataAsset);
	TArray<FName> NewKeyNames;
	for (const auto InputTip : InputTipsDataAsset->DisplayInputTips)
	{
		bool bIsNew;
		AddInputTips(InputTipsDataAsset->IdentityName, InputTip.Key, InputTip.Value, bIsNew);
		if (bIsNew)
		{
			NewKeyNames.Add(InputTip.Key);
		}
	}
	OnDisplayKeysAddedEvent.Broadcast(NewKeyNames);
}

void UInputTipsSubsystem::RemoveInputTipsDataAsset(UInputTipsDataAsset* InputTipsDataAsset)
{
	if (!InputTipsDataAsset)
	{
		return;
	}
	
	DisplayingInputTipsDataAssets.Remove(InputTipsDataAsset);
	RemoveInputTipsByName(InputTipsDataAsset->IdentityName);
}

void UInputTipsSubsystem::AddSingleInputTips(const FName IdentityName, const FName KeyName,
	const FTagInputTipsArray& InTipsQuery)
{
	bool bIsNew;
	AddInputTips(IdentityName, KeyName, InTipsQuery, bIsNew);
	if (bIsNew)
	{
		OnDisplayKeysAddedEvent.Broadcast({KeyName});
	}
}

void UInputTipsSubsystem::RemoveInputTipsByName(const FName IdentityName)
{
	TArray<FName> RemovedKeyNames;
	RemoveInputTips(IdentityName, RemovedKeyNames);
	if (!RemovedKeyNames.IsEmpty())
	{
		OnDisplayKeysRemovedEvent.Broadcast(RemovedKeyNames);
	}
}

void UInputTipsSubsystem::SetKeyAutoClear(const FName KeyName, const bool bAutoClear)
{
	if (const auto Found = DisplayingInputTips.Find(KeyName))
	{
		Found->bAutoClear = bAutoClear;
	}
}

void UInputTipsSubsystem::AddInputTips(const FName& IdentityName, const FName& KeyName,
                                       const FTagInputTipsArray& InTipsQuery, bool& bIsNew)
{
	bIsNew = false;
	if (!IdentityName.IsValid() || !KeyName.IsValid())
	{
		return;
	}
	
	if (const auto FoundTips = DisplayingInputTips.Find(KeyName))
	{
		auto Copy = InTipsQuery.TipsTagQuery;
		for (auto& Tip : Copy)
		{
			Tip.IdentityName = IdentityName;
		}
		FoundTips->TipsTagQuery.Append(Copy);
	}
	else
	{
		auto Copy = InTipsQuery.TipsTagQuery;
		for (auto& Tip : Copy)
		{
			Tip.IdentityName = IdentityName;
		}
		DisplayingInputTips.Add(KeyName, FTagInputTipsArray(InTipsQuery.bAutoClear, Copy));
		bIsNew = true;
	}
}

void UInputTipsSubsystem::RemoveInputTips(const FName& IdentityName, TArray<FName>& EmptyTips)
{
	EmptyTips.Empty();
	
	for (auto& Tip : DisplayingInputTips)
	{
		Tip.Value.TipsTagQuery.RemoveAll([IdentityName](const FTagInputTips& InTips)
		{
			return InTips.IdentityName == IdentityName;
		});
		
		if (Tip.Value.TipsTagQuery.IsEmpty() && Tip.Value.bAutoClear)
		{
			EmptyTips.Add(Tip.Key);
		}
	}

	for (const auto Name : EmptyTips)
	{
		DisplayingInputTips.Remove(Name);
	}
}

bool UInputTipsSubsystem::GetMappingKeyName(FName KeyName, FText& DisplayKeyName)
{
	if (!EnhancedInputSubsystem) return false;
	
	auto Mappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
	const auto Index = FastQueryMap.Find(KeyName);
	if (Index && Mappings.IsValidIndex(*Index) && Mappings[*Index].GetPlayerMappableKeySettings()
		&& Mappings[*Index].GetPlayerMappableKeySettings()->Name == KeyName)
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
	if (Index && Mappings.IsValidIndex(*Index) && Mappings[*Index].GetPlayerMappableKeySettings()
		&& Mappings[*Index].GetPlayerMappableKeySettings()->Name == KeyName)
	{
		// Use current runtime query.
		if (QueryDisplayInputTips(KeyName, TagsContainerObject, DisplayName, bShouldDisplay))
		{
			return true;
		}
		
		// Fallback to enhanced input mapping context display name.
		DisplayName = Mappings[*Index].GetPlayerMappableKeySettings()->DisplayName;
		bShouldDisplay = true;
		return true;
	}

	return false;
}

bool UInputTipsSubsystem::QueryDisplayInputTips(const FName KeyName, const UObject* TagsContainerObject, FText& DisplayInputTips, bool& bShouldDisplay)
{
	for (const auto Mapping : DisplayingInputTips)
	{
		if (Mapping.Key == KeyName)
		{
			return Mapping.Value.GetDisplayTips(TagsContainerObject, DisplayInputTips, bShouldDisplay);
		}
	}

	DisplayInputTips = FText();
	return false;
}
