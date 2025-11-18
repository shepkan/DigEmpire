#pragma once

#include "CoreMinimal.h"
#include "MapGenerationStepDataBase.h"
#include "ZoneConnectivityStepData.generated.h"

class UZoneBorderSettings;

/** Executes connectivity fix within each zone using border settings for wall tags. */
UCLASS(BlueprintType)
class UZoneConnectivityStepData : public UMapGenerationStepDataBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Connectivity")
    TObjectPtr<UZoneBorderSettings> BorderSettings;

    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};

