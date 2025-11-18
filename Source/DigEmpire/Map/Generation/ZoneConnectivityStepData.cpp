#include "ZoneConnectivityStepData.h"

#include "ZoneConnectivityFixer.h"
#include "ZoneBorderSettings.h"
#include "DigEmpire/Map/MapGrid2D.h"

void UZoneConnectivityStepData::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    const UZoneBorderSettings* BS = BorderSettings;
    if (!BS)
    {
        BS = Cast<UZoneBorderSettings>(StaticLoadObject(UZoneBorderSettings::StaticClass(), nullptr, TEXT("/Game/Map/DA_ZoneBorderSettings.DA_ZoneBorderSettings")));
    }
    if (!BS) return;

    UZoneConnectivityFixer* Fixer = NewObject<UZoneConnectivityFixer>();
    Fixer->Generate(Map, InOutZoneLabels, BS);
}
