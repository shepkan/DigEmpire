// CharacterGridVisionMessages.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "CharacterGridVisionMessages.generated.h"

class AActor;

/** Single grid cell with coordinates and data */
USTRUCT(BlueprintType)
struct FGridCellWithCoord
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FIntPoint Coord = FIntPoint::ZeroValue;

    UPROPERTY(BlueprintReadOnly)
    FMapCell Cell;
};

/** A single vision ring (cells at a given radius band). */
USTRUCT(BlueprintType)
struct FGridCellsRing
{
    GENERATED_BODY()

    /** Cells in this ring (e.g., center for ring 0; distance (n-1,n] for ring n>0). */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGridCellWithCoord> Cells;
};

/** Payload published by CharacterGridVision component */
USTRUCT(BlueprintType)
struct FCharacterGridVisionMessage
{
    GENERATED_BODY()

    /** Component owner sending this vision update */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> SourceActor = nullptr;

    /** Center cell (rounded from owner world location) */
    UPROPERTY(BlueprintReadOnly)
    FIntPoint Center = FIntPoint::ZeroValue;

    /** Vision radius in cells (euclidean) used for this update */
    UPROPERTY(BlueprintReadOnly)
    int32 RadiusCells = 0;

    /**
     * Nearby cells grouped by euclidean radius layers (in-bounds only).
     * Index 0: center cell; index 1: cells with distance (0,1]; index 2: (1,2]; ...
     */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGridCellsRing> RadiusLayers;
};

/** Cells that have been seen for the first time */
USTRUCT(BlueprintType)
struct FCellsFirstSeenMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> SourceActor = nullptr;

    /** Newly seen cells (each appears only once overall) */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGridCellWithCoord> Cells;
};
