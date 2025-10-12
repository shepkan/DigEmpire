#include "TileTextureSet.h"

UTexture2D* UTileTextureSet::FindBackgroundTexture(const FGameplayTag& Tag) const
{
	if (!Tag.IsValid()) return nullptr;
	for (const FTagTexturePair& P : Backgrounds)
	{
		if (P.Tag.MatchesTagExact(Tag))
		{
			return P.Texture;
		}
	}
	return nullptr;
}

UTexture2D* UTileTextureSet::FindObjectTexture(const FGameplayTag& Tag) const
{
	if (!Tag.IsValid()) return nullptr;
	for (const FTagTexturePair& P : Objects)
	{
		if (P.Tag.MatchesTagExact(Tag))
		{
			return P.Texture;
		}
	}
	return nullptr;
}
