#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RoomGenSettings.h"
#include "RoomGenerator.generated.h"

class UMapGrid2D;

/** Places rectangular rooms inside zones and builds walls around them, leaving a single entrance. */
UCLASS(BlueprintType)
class URoomGenerator : public UObject
{
    GENERATED_BODY()
public:
    /**
     * Reserve per-zone rectangles as rooms and place walls around them (like zone borders).
     * Returns true on success (placed as many rooms as possible).
     */
    UFUNCTION(BlueprintCallable, Category="Rooms")
    bool Generate(UMapGrid2D* MapGrid,
                  const TArray<int32>& ZoneLabels,
                  const URoomGenSettings* Settings);

private:
    static int32 Idx(int32 X, int32 Y, int32 W) { return X + Y * W; }
    bool TryPlaceRoomInZone(UMapGrid2D* Map,
                            const FIntPoint& Size,
                            int32 ZoneId,
                            int32 RoomW,
                            int32 RoomH,
                            const TArray<int32>& Labels,
                            const FGameplayTag& WallTag,
                            int32 WallHP,
                            int32 MaxAttempts,
                            FRandomStream& RNG);
};
