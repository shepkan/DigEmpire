#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/Generation/MapGenerationStepDataBase.h"
#include "DigEmpire/Map/CellActor.h"
#include "DigEmpire/Config/DEConstants.h"
#include "CellActorPlacementSettings.generated.h"

class UMapGrid2D;

/** Per-zone placement spec for a cell actor. */
USTRUCT(BlueprintType)
struct FZoneActorPlacement
{
    GENERATED_BODY()

    /** Actor class to spawn (must derive from ACellActor). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement")
    TSubclassOf<ACellActor> ActorClass;

    /** Zones where to place the actor. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement")
    TArray<int32> Zones;

    /** How many instances to place per zone. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement", meta=(ClampMin="0"))
    int32 CountPerZone = 0;

    /** If true, place only inside rooms of the zone; if false, place in zone cells excluding rooms. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement")
    bool bOnlyInRooms = true;
};

/** Settings for placing arbitrary cell actors by zone into empty cells. */
UCLASS(BlueprintType)
class UCellActorPlacementSettings : public UMapGenerationStepDataBase
{
    GENERATED_BODY()

public:
    /** List of per-zone placement specs. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement")
    TArray<FZoneActorPlacement> Placements;

    /** Cell-to-world scale for X/Y placement (uu per cell). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Placement", meta=(ClampMin="1.0"))
    float TileSizeUU = DEConstants::TileSizeUU;

    /** World Z to place spawned actors at. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Placement")
    float ZOffsetUU = 0.f;

    /** Random seed; if < 0 a random seed is used. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Random")
    int32 RandomSeed = -1;

    // Execute step: place actors in specified zones into empty cells
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};

