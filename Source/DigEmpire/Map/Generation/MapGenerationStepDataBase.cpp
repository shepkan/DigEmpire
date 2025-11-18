#include "MapGenerationStepDataBase.h"

#include "DigEmpire/Map/MapGrid2D.h"

void UMapGenerationStepDataBase::ExecuteGenerationStep(UMapGrid2D* /*Map*/, UWorld* /*World*/, TArray<int32>& /*InOutZoneLabels*/) const
{
    // Default base does nothing.
}

