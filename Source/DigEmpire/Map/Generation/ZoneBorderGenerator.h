#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneBorderSettings.h"
#include "ZonePassageTypes.h"
#include "ZoneBorderGenerator.generated.h"

class UMapGrid2D;

UCLASS(BlueprintType)
class UZoneBorderGenerator : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ZoneBorders")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const UZoneBorderSettings* Settings);

    /** Returns FREE (no-object) cells of a single zone. */
    UFUNCTION(BlueprintPure, Category="ZoneBorders|Query")
    TArray<FIntPoint> GetFreeCellsForZone(UMapGrid2D* MapGrid, int32 ZoneId) const;

private:
    UPROPERTY(Transient) TArray<int32> CachedLabels;
    UPROPERTY(Transient) FIntPoint CachedSize = FIntPoint::ZeroValue;

    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }
    inline bool InBounds(int32 X, int32 Y) const { return X>=0 && Y>=0 && X<CachedSize.X && Y<CachedSize.Y; }

    bool ValidateInputs(const UMapGrid2D* Map, const TArray<int32>& Labels, const UZoneBorderSettings* Settings) const;

    void CollectZoneBoundaries(const TArray<int32>& Labels,
                               TMap<FIntPoint, TSet<FIntPoint>>& OutPairToA,
                               TMap<FIntPoint, TSet<FIntPoint>>& OutPairToB) const;

    void PlaceWallsWithThickness(UMapGrid2D* Map,
                                 const UZoneBorderSettings* Settings,
                                 const TMap<FIntPoint, TSet<FIntPoint>>& PairToA);

    void ExpandInwardWithinZone(const TSet<FIntPoint>& Seeds,
                                int32 ZoneId,
                                int32 Rings,
                                TSet<FIntPoint>& OutExpanded) const;

    /** Map utilities */
    void PutWall(UMapGrid2D* Map, int32 X, int32 Y, const UZoneBorderSettings* Settings) const;
};

