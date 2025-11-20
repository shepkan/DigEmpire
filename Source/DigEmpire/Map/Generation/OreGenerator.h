#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OreGenSettings.h"
#include "OreGenerator.generated.h"

class UMapGrid2D;

/** Places ore on blocked (object-occupied) cells per zone using configured count ranges. */
UCLASS(BlueprintType)
class UOreGenerator : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Ore")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const UOreGenSettings* Settings);

private:
    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }
};

