#include "CellActorPlacementSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "CellActorPlacer.h"

void UCellActorPlacementSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* World, TArray<int32>& /*InOutZoneLabels*/) const
{
    if (!Map || !World) return;
    if (Placements.Num() == 0) return;
    UCellActorPlacer* Placer = NewObject<UCellActorPlacer>();
    Placer->Generate(Map, this, World);
}

