#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/Generation/MapGenerationStepDataBase.h"
#include "DigEmpire/Map/CellActor.h"
#include "ZoneDoorSettings.generated.h"

/**
 * Settings for placing doors in zone passages.
 * Provides the actor class and optional transform parameters.
 */
UCLASS(BlueprintType)
class UZoneDoorSettings : public UMapGenerationStepDataBase
{
    GENERATED_BODY()

public:
    /** Actor class to spawn for a door (must derive from ACellActor). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
    TSubclassOf<ACellActor> DoorClass;

    /** Cell-to-world scale for X/Y placement (uu per cell). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door", meta=(ClampMin="1.0"))
    float TileSizeUU = 100.f;

    /** World Z to place spawned doors at. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
    float ZOffsetUU = 0.f;

    /** Actor class to spawn for a key (must derive from ACellActor, ideally AKeyCellActor). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Key")
    TSubclassOf<ACellActor> KeyClass;

    /** Optional per-zone color tags for doors/keys. ZoneId -> DoorColor tag. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Key")
    TMap<int32, FGameplayTag> ZoneColorTags;

    // Execute step: place door actors along passages
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
