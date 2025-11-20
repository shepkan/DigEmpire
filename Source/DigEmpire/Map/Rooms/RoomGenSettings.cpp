#include "RoomGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "RoomGenerator.h"
// No longer depends on ZoneBorderSettings

void URoomGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    URoomGenerator* RoomGen = NewObject<URoomGenerator>();
    RoomGen->Generate(Map, InOutZoneLabels, this);
}
