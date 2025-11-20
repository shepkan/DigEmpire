#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/Generation/MapGenerationStepDataBase.h"
#include "GameplayTagContainer.h"
#include "OreGenSettings.generated.h"

class UMapGrid2D;

/** Single ore spec with count range. */
USTRUCT(BlueprintType)
struct FOreCountRange
{
    GENERATED_BODY()

    /** Ore gameplay tag to assign. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore")
    FGameplayTag OreTag;

    /** Minimum number of this ore to place. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore", meta=(ClampMin="0"))
    int32 MinCount = 0;

    /** Maximum number of this ore to place (inclusive). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore", meta=(ClampMin="0"))
    int32 MaxCount = 0;
};

/** Per-zone list of ore specs. ZoneIndex is 0-based. */
USTRUCT(BlueprintType)
struct FZoneOreConfig
{
    GENERATED_BODY()

    /** Zone index this config applies to (0-based). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore")
    int32 ZoneIndex = 0;

    /** Ores to place with count ranges. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore")
    TArray<FOreCountRange> Ores;
};

/** Settings for placing ore on blocked tiles inside each zone. */
UCLASS(BlueprintType)
class UOreGenSettings : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
    /** Per-zone ores config (zone index -> list of ores with ranges). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore")
    TArray<FZoneOreConfig> ZoneOres;

    /** Object tags (blocks) where ore must NOT be generated. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ore")
    TArray<FGameplayTag> ForbiddenObjectTags;

    /** Random seed; if < 0 a random seed is used. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Random")
    int32 RandomSeed = -1;

    // Execute step: place ores per zone on blocked (object-occupied) tiles
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
