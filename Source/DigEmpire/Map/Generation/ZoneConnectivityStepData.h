#pragma once

#include "CoreMinimal.h"
#include "MapGenerationStepDataBase.h"
#include "GameplayTagContainer.h"
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

    virtual void ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const override;
};
