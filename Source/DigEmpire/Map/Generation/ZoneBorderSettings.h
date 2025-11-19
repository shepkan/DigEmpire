#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/Generation/MapGenerationStepDataBase.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/Config/DEConstants.h"
#include "ZoneBorderSettings.generated.h"

/** Optional per-zone cap: how many passages (degree) a zone may have. */
USTRUCT(BlueprintType)
struct FZonePassageCap
{
	GENERATED_BODY()

	/** Zone index (0-based). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages")
	int32 ZoneIndex = 0;

	/** Max passages incident to this zone; -1 = unlimited. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages")
	int32 MaxPassages = -1;
};

/**
 * Settings for placing walls along zone borders with one perpendicular passage per adjacent zone pair.
 * Degree caps and global passage spacing are supported.
 */
UCLASS(BlueprintType)
class UZoneBorderSettings : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
	/** Object tag to place as a wall (blocking). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Borders")
	FGameplayTag WallObjectTag;

	/** Durability of the wall object (>0 means blocking). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Borders", meta=(ClampMin="1"))
	int32 WallDurability = 100;

	/** Border thickness in cells on ZoneA (lower id) side. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Borders", meta=(ClampMin="1"))
	int32 BorderThickness = 1;

	/** Passage width in cells along the boundary tangent. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages", meta=(ClampMin="1"))
	int32 PassageWidth = 1;

	/** Minimal Manhattan distance (in cells) required between passages. 0 = disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages", meta=(ClampMin="0"))
	int32 MinPassageDistance = 2;
	
	/** How many anchors to try per adjacent zone pair when carving a passage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages", meta=(ClampMin="1"))
	int32 AttemptsPerPair = 6;
	
	/** Random seed; if < 0 a random seed is used (FMath::Rand()). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Random")
	int32 RandomSeed = -1;

	/** Per-zone degree caps (optional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Passages")
	TArray<FZonePassageCap> DegreeCaps;

	/** Debug draw chosen passage cells. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	bool bDebugDrawPassages = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float DebugTileSizeUU = DEConstants::TileSizeUU;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	float DebugZOffset = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	float DebugSphereRadiusUU = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    float DebugLifetime = 5.f;

    // Execute step: place walls on zone borders and carve passages
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
