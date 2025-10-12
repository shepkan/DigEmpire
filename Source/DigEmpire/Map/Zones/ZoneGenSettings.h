#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ZoneGenSettings.generated.h"

/** Per-zone weight item (relative size target). */
USTRUCT(BlueprintType)
struct FZoneWeight
{
	GENERATED_BODY()

	/** Zone index (0-based). Zone 0 is the "Zone 1" from your description. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zones")
	int32 ZoneIndex = 0;

	/** Relative weight: e.g., 20, 10, 10, 5. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zones", meta=(ClampMin="0"))
	int32 Weight = 1;
};

/**
 * Settings for weighted multi-source region growing.
 * Controls seeds, soft quotas, non-touch constraints and debug draw.
 */
UCLASS(BlueprintType)
class UZoneGenSettings : public UDataAsset
{
	GENERATED_BODY()
public:
	/** List of zone weights; length = number of zones. ZoneIndex should be 0..NumZones-1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zones")
	TArray<FZoneWeight> ZoneWeights;

	/** Zones that are NOT allowed to touch Zone 0 (aka "Zone 1" in your wording). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Constraints")
	TArray<int32> ForbiddenFromZone0;

	/** Optional moat (in cells) around Zone 0 where forbidden zones cannot claim. 0 = disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Constraints", meta=(ClampMin="0"))
	int32 MoatFromZone0 = 0;

	/** Minimum seed separation (in cells). Generator will try to place zone seeds at least this far apart. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Seeds", meta=(ClampMin="0"))
	int32 MinSeedSeparation = 6;

	/** Random stream seed for deterministic generation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Random")
	int32 RandomSeed = -1;

	/** Softness factor: extra chance to grow even if target reached (0..1 typical). Lower = harder quotas. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Quotas", meta=(ClampMin="0.0"))
	float SoftnessK = 0.25f;

	/** Allow slight overshoot over target before zone effectively stops growing (e.g., 1.08 = +8%). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Quotas", meta=(ClampMin="1.0"))
	float OverfillFactor = 1.08f;

	/** Tile size in world units — only used for debug draw placement. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	float TileSizeUU = 100.f;

	/** Enable debug spheres at each cell center, colored by zone id. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	bool bDebugDraw = false;

	/** Sphere radius in UU for debug draw. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug", meta=(ClampMin="0.0"))
	float DebugSphereRadiusUU = 15.f;

	/** Sphere lifetime in seconds (0 = persistent). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug", meta=(ClampMin="0.0"))
	float DebugLifetime = 5.f;

	/** Z offset for debug spheres. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
	float DebugZOffset = 10.f;
};
