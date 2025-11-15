#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DigEmpire/Map/CellActor.h"
#include "ZoneDoorSettings.generated.h"

/**
 * Settings for placing doors in zone passages.
 * Provides the actor class and optional transform parameters.
 */
UCLASS(BlueprintType)
class UZoneDoorSettings : public UDataAsset
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
};

