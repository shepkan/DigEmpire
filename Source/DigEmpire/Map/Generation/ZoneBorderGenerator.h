#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneBorderSettings.h"
#include "ZoneBorderGenerator.generated.h"

class UMapGrid2D;

USTRUCT(BlueprintType)
struct FZonePassage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Passage")
	int32 ZoneA = 0; // min id

	UPROPERTY(BlueprintReadOnly, Category="Passage")
	int32 ZoneB = 0; // max id

	UPROPERTY(BlueprintReadOnly, Category="Passage")
	TArray<FIntPoint> Cells; // carved cells (both sides)
};

UCLASS(BlueprintType)
class UZoneBorderGenerator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="ZoneBorders")
	bool Generate(UMapGrid2D* MapGrid,
	              const TArray<int32>& ZoneLabels,
	              const UZoneBorderSettings* Settings);

	UFUNCTION(BlueprintPure, Category="ZoneBorders|Query")
	const TArray<FZonePassage>& GetPassages() const { return Passages; }

	/** Returns FREE (no-object) cells of a single zone. */
	UFUNCTION(BlueprintPure, Category="ZoneBorders|Query")
	TArray<FIntPoint> GetFreeCellsForZone(UMapGrid2D* MapGrid, int32 ZoneId) const;

private:
	UPROPERTY(Transient) TArray<int32> CachedLabels;
	UPROPERTY(Transient) FIntPoint CachedSize = FIntPoint::ZeroValue;
	UPROPERTY(Transient) TArray<FZonePassage> Passages;

	/** Global protection mask for passages (dilated to protect against walls and spacing). */
	UPROPERTY(Transient) TSet<FIntPoint> PassageMask;

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

	void PlaceWallsWithThickness(UMapGrid2D* Map,
	                             const UZoneBorderSettings* Settings,
	                             const TMap<FIntPoint, TSet<FIntPoint>>& PairToA);

	void ExpandInwardWithinZone(const TSet<FIntPoint>& Seeds,
	                            int32 ZoneId,
	                            int32 Rings,
	                            TSet<FIntPoint>& OutExpanded) const;

	/** Dilate a mask by Manhattan radius. */
	void DilateMask(TSet<FIntPoint>& InOutMask, int32 Radius) const;

	/** Returns true if any candidate cell is within MinDist of current PassageMask. */
	bool IsTooCloseToExistingPassages(const TArray<FIntPoint>& CandidateCells, int32 MinDist) const;
	
	// ZoneBorderGenerator.h (private)
	bool BuildCarveStripePreview(
		UMapGrid2D* Map,
		const FIntPoint& AnchorA,
		bool bVerticalBoundary,      // true => passage tangent is Y; false => tangent is X
		int dxToB, int dyToB,        // vector from A to B across the boundary (±1,0) or (0,±1)
		int32 PassageWidth,
		int32 ZoneA, int32 ZoneB,
		TArray<FIntPoint>& OutCellsA, // wall cells to clear on A side
		TArray<FIntPoint>& OutCellsB  // wall cells to clear on B side
	) const;

	bool IsOccupied(UMapGrid2D* Map, int32 X, int32 Y) const; // any blocking object present

	/** Map utilities */
	void PutWall(UMapGrid2D* Map, int32 X, int32 Y, const UZoneBorderSettings* Settings) const;
	void ClearCell(UMapGrid2D* Map, int32 X, int32 Y) const;
	void DebugDrawPassageCells(const UZoneBorderSettings* S, const TArray<FIntPoint>& Cells, const FColor& Color, UWorld* World) const;
};
