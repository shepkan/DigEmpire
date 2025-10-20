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

    /** Nearby cells within radius (in-bounds only) */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGridCellWithCoord> Cells;
};

