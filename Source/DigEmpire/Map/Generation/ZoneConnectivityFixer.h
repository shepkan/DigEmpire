#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneBorderSettings.h"
#include "ZoneConnectivityFixer.generated.h"

class UMapGrid2D;

/**
 * For each zone: picks the largest connected open component and
 * carves minimal corridors (within the same zone) to connect all
 * other open components to it. Does not remove walls between zones
 * and does not remove room walls.
 */
UCLASS(BlueprintType)
class UZoneConnectivityFixer : public UObject
{
    GENERATED_BODY()
  public:
    // Returns true if all open cells (no wall object) in the zone are connected, including passage cells.
    UFUNCTION(BlueprintPure, Category="Zones")
    bool IsZoneConnected(const UMapGrid2D* MapGrid,
                         int32 ZoneId) const;
    UFUNCTION(BlueprintCallable, Category="Zones")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const UZoneBorderSettings* BorderSettings);

private:
    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }
    struct FNode { int16 X, Y; };

    void BuildMasksForZone(UMapGrid2D* Map,
                           const TArray<int32>& Labels,
                           int32 ZoneId,
                           const UZoneBorderSettings* BorderSettings,
                           /*out*/ TArray<uint8>& Open,      // 1=open, 0=not
                           /*out*/ TArray<uint8>& ImmWall,   // 1=immutable wall
                           /*out*/ TArray<uint8>& MutWall);  // 1=mutable wall
};
