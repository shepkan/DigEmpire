#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneGenSettings.h"
#include "MapZoneGenerator.generated.h"

class UWorld;
class UMapGrid2D;

/**
 * Weighted multi-source region growing over UMapGrid2D.
 * - Honors soft quotas derived from ZoneWeights (relative sizes).
 * - Forbids contact (and optional moat) between Zone 0 and configured zones.
 * - Optionally draws debug spheres per cell, colored by zone id.
 *
 * Usage:
 *   UMapZoneGenerator* Gen = NewObject<UMapZoneGenerator>();
 *   TArray<int32> ZoneLabels; // length = SizeX*SizeY, will be filled with zone indices [0..Z-1]
 *   Gen->Generate(MapObj, SettingsAsset, GetWorld(), ZoneLabels);
 */
UCLASS(BlueprintType)
class UMapZoneGenerator : public UObject
{
	GENERATED_BODY()

public:
	/** Run generation. Returns true on success. OutZoneLabels is sized to W*H and filled with zone indices. */
	UFUNCTION(BlueprintCallable, Category="ZoneGen")
	bool Generate(UMapGrid2D* MapGrid,
	              const UZoneGenSettings* Settings,
	              UWorld* WorldForDebugDraw,
	              TArray<int32>& OutZoneLabels);

private:
	struct FSeed { int32 X=0, Y=0, Zone=0; };
	struct FFrontierEntry { int32 X=0, Y=0; };

	// Internal helpers
	bool ValidateInputs(UMapGrid2D* Map, const UZoneGenSettings* Settings) const;
	void ComputeTargets(int32 NumCells, const TArray<FZoneWeight>& Weights, TArray<int32>& OutTargets) const;
	void PlaceSeedsDeterministic(const FIntPoint& Size, const UZoneGenSettings* Settings, TArray<FSeed>& OutSeeds) const;
	int32 Idx(int32 X, int32 Y, int32 W) const { return X + Y * W; }

	// Growth step and constraints
	bool ViolatesForbiddenAdjacency(int32 ClaimZone, int32 X, int32 Y,
	                                const FIntPoint& Size,
	                                const TArray<int32>& Labels,
	                                const TSet<int32>& ForbiddenSet) const;
	bool WithinMoatForbidden(int32 ClaimZone, int32 X, int32 Y,
	                         const FIntPoint& Size,
	                         const TArray<int32>& DistFromZone0,
	                         const UZoneGenSettings* Settings,
	                         const TSet<int32>& ForbiddenSet) const;

	void PushFreeNeighborsToFrontier(int32 X, int32 Y,
	                                 const FIntPoint& Size,
	                                 const TArray<int32>& Labels,
	                                 TArray<FFrontierEntry>& Frontier) const;

	void ComputeDistFromZone0(const FIntPoint& Size,
	                          const TArray<int32>& Labels,
	                          TArray<int32>& OutDist) const;

	// Debug drawing
	void DebugDrawZones(const FIntPoint& Size,
	                    const TArray<int32>& Labels,
	                    const UZoneGenSettings* Settings,
	                    UWorld* World) const;
};
