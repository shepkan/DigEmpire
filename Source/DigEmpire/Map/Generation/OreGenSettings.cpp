#include "OreGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "OreGenerator.h"

void UOreGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    UOreGenerator* Gen = NewObject<UOreGenerator>();
    Gen->Generate(Map, InOutZoneLabels, this);
}

