#pragma once

#include "CoreMinimal.h"

#include "ZonePassageTypes.generated.h"

USTRUCT(BlueprintType)
struct FZonePassage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Passage")
    int32 ZoneA = 0; // min id

    UPROPERTY(BlueprintReadOnly, Category="Passage")
    int32 ZoneB = 0; // max id

    UPROPERTY(BlueprintReadOnly, Category="Passage")
    TArray<FIntPoint> Cells; // carved cells (both sides)
};

