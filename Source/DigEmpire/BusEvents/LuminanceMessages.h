// LuminanceMessages.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "LuminanceMessages.generated.h"

class AActor;

/**
 * Payload for per-cell luminance updates applied by the renderer.
 * - Provides current visible cells grouped by rings and the luminance per ring
 * - Also includes cells that left visibility to reset their luminance to default
 */
USTRUCT(BlueprintType)
struct FLuminanceUpdateMessage
{
    GENERATED_BODY()

    /** Source actor that owns the vision/light (for context/filtering if needed). */
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> SourceActor = nullptr;

    /** Visible cells grouped by euclidean rings (0=center). */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGridCellsRing> RadiusLayers;

    /** Luminance value per ring index; size should be >= RadiusLayers.Num(). */
    UPROPERTY(BlueprintReadOnly)
    TArray<float> LuminanceByRing;

    /** Cells that left visibility since previous update (set to renderer default). */
    UPROPERTY(BlueprintReadOnly)
    TArray<FIntPoint> CellsLeftVisibility;
};

