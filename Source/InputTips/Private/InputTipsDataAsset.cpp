// Fill out your copyright notice in the Description page of Project Settings.


#include "InputTipsDataAsset.h"

#include "GameplayTagAssetInterface.h"

bool FTagInputTipsArray::GetDisplayTips(const UObject* TagAssetObject, FText& DisplayTips, bool& bDisplay) const
{
	if (const auto Interface = Cast<IGameplayTagAssetInterface>(TagAssetObject))
	{
		for (const auto Itr : TipsTagQuery)
		{
			FGameplayTagContainer Container;
			Interface->GetOwnedGameplayTags(Container);
			if (Itr.Query.Matches(Container))
			{
				bDisplay = Itr.bDisplay;
				DisplayTips = Itr.DisplayTips;
				return true;
			}
		}
	}

	return false;
}
