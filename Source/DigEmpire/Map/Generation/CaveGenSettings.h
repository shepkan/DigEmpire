#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/Generation/MapGenerationStepDataBase.h"
#include "GameplayTagContainer.h"
#include "CaveGenSettings.generated.h"

/** Settings for per-zone cellular automata cave generation. */
UCLASS(BlueprintType)
class UCaveGenSettings : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
    /** Initial wall fill probability [0..1]. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Init", meta=(ClampMin="0.0", ClampMax="1.0"))
    float FillChance = 0.45f;

    /** Number of smoothing iterations. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Iter", meta=(ClampMin="0"))
    int32 Iterations = 5;

    /** Birth rule threshold (empty -> wall if neighbors >= BirthLimit). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Rules", meta=(ClampMin="0", ClampMax="8"))
    int32 BirthLimit = 5;

    /** Survival rule threshold (wall stays wall if neighbors >= SurvivalLimit). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Rules", meta=(ClampMin="0", ClampMax="8"))
    int32 SurvivalLimit = 4;

    /** Random seed; if < 0 a random seed is used. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Random")
    int32 RandomSeed = -1;

    /** Tag to place for cave walls produced by CA. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Walls")
    FGameplayTag WallObjectTag;

    /** Durability to place for cave walls. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Walls", meta=(ClampMin="1"))
    int32 WallDurability = 100;

    /** Tags treated as immutable walls when building CA masks (not modified by CA). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cave|Masks")
    TArray<FGameplayTag> ImmutableObjectTags;

    // Execute step: run cave CA per zone
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
