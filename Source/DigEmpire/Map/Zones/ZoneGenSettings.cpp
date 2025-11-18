#include "ZoneGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "MapZoneGenerator.h"

void UZoneGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    UMapZoneGenerator* Gen = NewObject<UMapZoneGenerator>();
    InOutZoneLabels.Reset();
    if (Gen->Generate(Map, this, World, InOutZoneLabels))
    {
        Map->ApplyZoneLabels(InOutZoneLabels);
    }
}

