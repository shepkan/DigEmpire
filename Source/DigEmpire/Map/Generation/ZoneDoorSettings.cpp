#include "ZoneDoorSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "ZoneDoorPlacer.h"

void UZoneDoorSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& /*InOutZoneLabels*/) const
{
    if (!Map || !World) return;
    if (!DoorClass) return;
    UZoneDoorPlacer* Placer = NewObject<UZoneDoorPlacer>();
    Placer->Generate(Map, this, World);
}

