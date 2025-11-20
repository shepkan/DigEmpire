#pragma once

#include "CoreMinimal.h"
#include "MapGenerationStepDataBase.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/Config/DEConstants.h"
#include "ZoneConnectivityStepData.generated.h"

/** Executes connectivity fix within each zone using border settings for wall tags. */
UCLASS(BlueprintType)
class UZoneConnectivityStepData : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
    /** Tags considered immutable walls (not to be removed while connecting). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Connectivity")
    TArray<FGameplayTag> ImmutableObjectTags;

    /** Debug: draw spheres for still-unconnected open cells after fix. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    bool bDebugDrawUnconnected = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    float DebugTileSizeUU = DEConstants::TileSizeUU;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    float DebugZOffset = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    float DebugSphereRadiusUU = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug")
    float DebugLifetime = 5.f;

    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
