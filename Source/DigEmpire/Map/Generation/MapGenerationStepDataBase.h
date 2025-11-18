#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapGenerationStepDataBase.generated.h"

class UMapGrid2D;
class UWorld;

/**
 * Base data asset for a single map generation step.
 * Derived assets override ExecuteGenerationStep to perform their logic.
 */
UCLASS(Abstract, BlueprintType, EditInlineNew)
class UMapGenerationStepDataBase : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Execute this generation step. Can read/update the map and zone labels. */
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const;
};
