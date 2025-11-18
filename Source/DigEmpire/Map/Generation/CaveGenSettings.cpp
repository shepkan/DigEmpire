#include "CaveGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "CaveGenerator.h"
#include "ZoneBorderSettings.h"

void UCaveGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    const UZoneBorderSettings* BS = BorderSettings;
    if (!BS)
    {
        BS = Cast<UZoneBorderSettings>(StaticLoadObject(UZoneBorderSettings::StaticClass(), nullptr, TEXT("/Game/Map/DA_ZoneBorderSettings.DA_ZoneBorderSettings")));
    }
    if (!BS) return;

    UCaveGenerator* CaveGen = NewObject<UCaveGenerator>();
    CaveGen->Generate(Map, InOutZoneLabels, this, BS);
}
