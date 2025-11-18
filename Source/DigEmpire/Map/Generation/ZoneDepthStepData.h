#pragma once

#include "CoreMinimal.h"
#include "MapGenerationStepDataBase.h"
#include "ZoneDepthStepData.generated.h"

/** Computes per-zone depth from Zone 0 via passages and stores it on the component. */
UCLASS(BlueprintType)
class UZoneDepthStepData : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};

