#include "ZoneBorderSettings.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "ZoneBorderGenerator.h"
#include "ZonePassageGenerator.h"

void UZoneBorderSettings::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() <= 0)
    {
        // Requires zone labels from a previous step
        return;
    }

    UZoneBorderGenerator* BorderGen = NewObject<UZoneBorderGenerator>();
    if (BorderGen->Generate(Map, InOutZoneLabels, this))
    {
        UZonePassageGenerator* PassageGen = NewObject<UZonePassageGenerator>();
        if (PassageGen->Generate(Map, InOutZoneLabels, this))
        {
            Map->SetPassages(PassageGen->GetPassages());
        }
    }
}

