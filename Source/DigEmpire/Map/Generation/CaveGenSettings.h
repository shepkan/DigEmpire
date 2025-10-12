#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CaveGenSettings.generated.h"

/** Settings for per-zone cellular automata cave generation. */
UCLASS(BlueprintType)
class UCaveGenSettings : public UDataAsset
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
};

