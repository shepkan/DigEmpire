#include "CaveGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "CaveGenerator.h"
// No longer depends on ZoneBorderSettings

void UCaveGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    UCaveGenerator* CaveGen = NewObject<UCaveGenerator>();
    CaveGen->Generate(Map, InOutZoneLabels, this);
}
