#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TileTextureSet.generated.h"

class UTexture2D;

/** Mapping item: GameplayTag -> Texture */
USTRUCT(BlueprintType)
struct FTagTexturePair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile")
	TObjectPtr<UTexture2D> Texture = nullptr;
};

/**
 * Data asset that holds textures for background and object tags.
 * It is used by the sprite renderer to resolve which texture to draw for each cell.
 */
UCLASS(BlueprintType)
class UTileTextureSet : public UDataAsset
{
	GENERATED_BODY()
public:
	/** Background tag -> texture mapping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TileTextures")
	TArray<FTagTexturePair> Backgrounds;

	/** Object tag -> texture mapping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TileTextures")
	TArray<FTagTexturePair> Objects;

	/** Find background texture by tag (nullptr if not found) */
	UFUNCTION(BlueprintPure, Category="TileTextures")
	UTexture2D* FindBackgroundTexture(const FGameplayTag& Tag) const;

	/** Find object texture by tag (nullptr if not found) */
	UFUNCTION(BlueprintPure, Category="TileTextures")
	UTexture2D* FindObjectTexture(const FGameplayTag& Tag) const;
};
