#include "RoomGenSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "RoomGenerator.h"
#include "DigEmpire/Map/Generation/ZoneBorderSettings.h"

void URoomGenSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0) return;
    const UZoneBorderSettings* BS = BorderSettings;
    if (!BS)
    {
        // Fallback to default asset path if not set in the asset
        BS = Cast<UZoneBorderSettings>(StaticLoadObject(UZoneBorderSettings::StaticClass(), nullptr, TEXT("/Game/Map/DA_ZoneBorderSettings.DA_ZoneBorderSettings")));
    }
    if (!BS) return;

    URoomGenerator* RoomGen = NewObject<URoomGenerator>();
    RoomGen->Generate(Map, InOutZoneLabels, this, BS);
}
