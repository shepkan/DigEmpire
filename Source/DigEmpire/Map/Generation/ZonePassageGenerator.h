#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneBorderSettings.h"
#include "ZonePassageTypes.h"
#include "ZonePassageGenerator.generated.h"

class UMapGrid2D;

UCLASS(BlueprintType)
class UZonePassageGenerator : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ZonePassages")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const UZoneBorderSettings* Settings);

    UFUNCTION(BlueprintPure, Category="ZonePassages|Query")
    const TArray<FZonePassage>& GetPassages() const { return Passages; }

private:
    UPROPERTY(Transient) TArray<int32> CachedLabels;
    UPROPERTY(Transient) FIntPoint CachedSize = FIntPoint::ZeroValue;
    UPROPERTY(Transient) TArray<FZonePassage> Passages;
    UPROPERTY(Transient) TSet<FIntPoint> PassageMask; // global protection mask

    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }
    inline bool InBounds(int32 X, int32 Y) const { return X>=0 && Y>=0 && X<CachedSize.X && Y<CachedSize.Y; }

    bool ValidateInputs(const UMapGrid2D* Map, const TArray<int32>& Labels, const UZoneBorderSettings* Settings) const;
    void CollectZoneBoundaries(const TArray<int32>& Labels,
                               TMap<FIntPoint, TSet<FIntPoint>>& OutPairToA,
                               TMap<FIntPoint, TSet<FIntPoint>>& OutPairToB) const;

    void ChooseAndCarvePassages(UMapGrid2D* Map,
                                const UZoneBorderSettings* Settings,
                                const TMap<FIntPoint, TSet<FIntPoint>>& PairToA,
                                const TMap<FIntPoint, TSet<FIntPoint>>& PairToB);

    // Helpers for carving
    void ClearCell(UMapGrid2D* Map, int32 X, int32 Y) const;
    bool IsTooCloseToExistingPassages(const TArray<FIntPoint>& CandidateCells, int32 MinDist) const;
    void DilateMask(TSet<FIntPoint>& InOutMask, int32 Radius) const;
    bool BuildCarveStripePreview(UMapGrid2D* Map,
                                 const FIntPoint& AnchorA,
                                 bool bVerticalBoundary,
                                 int dxToB, int dyToB,
                                 int32 PassageWidth,
                                 int32 ZoneA, int32 ZoneB,
                                 TArray<FIntPoint>& OutCellsA,
                                 TArray<FIntPoint>& OutCellsB) const;
    void DebugDrawPassageCells(const UZoneBorderSettings* S, const TArray<FIntPoint>& Cells, const FColor& Color, UWorld* World) const;
};

