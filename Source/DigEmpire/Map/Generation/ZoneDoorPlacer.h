#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZoneDoorPlacer.generated.h"

class UMapGrid2D;
class UZoneDoorSettings;

/** Places door actors at the mid-point of each carved passage. */
UCLASS(BlueprintType)
class UZoneDoorPlacer : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="ZonePassages|Doors")
    bool Generate(UMapGrid2D* MapGrid, const UZoneDoorSettings* Settings, UWorld* World);

private:
    static FIntPoint PickMidCell(const TArray<FIntPoint>& Cells);
};

