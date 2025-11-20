#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CaveGenSettings.h"
#include "CaveGenerator.generated.h"

class UMapGrid2D;

/** Per-zone cellular automata cave generator with immutable walls/rooms/passages. */
UCLASS(BlueprintType)
class UCaveGenerator : public UObject
{
    GENERATED_BODY()
public:
    /** Runs the CA for each zone, respecting immutable features. */
    UFUNCTION(BlueprintCallable, Category="Cave")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const UCaveGenSettings* Settings);

private:
    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }

    void BuildZoneMasks(UMapGrid2D* Map,
                        const TArray<int32>& Labels,
                        int32 ZoneId,
                        const UCaveGenSettings* Settings,
                        /*out*/ TArray<int8>& FixedState, // -1 mutable, 0 immutable empty, 1 immutable wall
                        /*out*/ TArray<uint8>& Cur);

    int32 CountNeighbors8(const TArray<int8>& Fixed,
                          const TArray<uint8>& Cur,
                          const TArray<int32>& Labels,
                          int32 ZoneId,
                          int32 X, int32 Y,
                          int32 W, int32 H) const;

    // Cached for current zone to bias walls near room walls
    TSet<FIntPoint> RoomWallCells;
};
