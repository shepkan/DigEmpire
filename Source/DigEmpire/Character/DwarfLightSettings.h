// DwarfLightSettings.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DwarfLightSettings.generated.h"

USTRUCT(BlueprintType)
struct FDwarfLightVisibilityConfig
{
    GENERATED_BODY()

    /** Vision radius (in cells) to apply at or above the threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
    int32 VisionRadius = 0;

    /** Per-ring luminance: index 0=center, 1=ring1, ..., size should be >= VisionRadius+1. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
    TArray<float> LuminanceByRing;
};

/**
 * Settings for DwarfLightComponent.
 * - Default LightPower value [0..100]
 * - Decrease per second
 * - Mapping from LightPower threshold -> vision radius (in grid cells)
 */
UCLASS(BlueprintType)
class UDwarfLightSettings : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Initial LightPower value applied at BeginPlay. [0..100] */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta=(ClampMin="0", ClampMax="100"))
    float DefaultLightPower = 100.f;

    /** How much LightPower decreases each second. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta=(ClampMin="0"))
    float DecreasePerSecond = 1.0f;

    /**
     * Settings at LightPower thresholds. The highest key <= current LightPower is used.
     * Map key is threshold; value is config with radius and luminance per ring.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
    TMap<int32, FDwarfLightVisibilityConfig> VisibilityRanges;

    /** Find the best config for a given LightPower (highest threshold <= Power). */
    const FDwarfLightVisibilityConfig* FindBestConfig(float Power) const
    {
        if (VisibilityRanges.Num() == 0) return nullptr;
        int32 BestThreshold = TNumericLimits<int32>::Min();
        const FDwarfLightVisibilityConfig* Best = nullptr;
        for (const auto& Pair : VisibilityRanges)
        {
            if (Pair.Key <= Power && Pair.Key > BestThreshold)
            {
                BestThreshold = Pair.Key;
                Best = &Pair.Value;
            }
        }
        if (!Best)
        {
            // fallback to the smallest threshold
            int32 SmallestKey = TNumericLimits<int32>::Max();
            for (const auto& Pair : VisibilityRanges)
            {
                if (Pair.Key < SmallestKey)
                {
                    SmallestKey = Pair.Key;
                    Best = &Pair.Value;
                }
            }
        }
        return Best;
    }
};
