#include "ZoneConnectivityStepData.h"

#include "ZoneConnectivityFixer.h"
#include "DigEmpire/Map/MapGrid2D.h"

void UZoneConnectivityStepData::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    UZoneConnectivityFixer* Fixer = NewObject<UZoneConnectivityFixer>();
    Fixer->Generate(Map, InOutZoneLabels, ImmutableObjectTags,
                    bDebugDrawUnconnected, DebugTileSizeUU, DebugZOffset, DebugSphereRadiusUU, DebugLifetime);
}
