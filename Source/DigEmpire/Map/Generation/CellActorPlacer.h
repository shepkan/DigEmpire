#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CellActorPlacer.generated.h"

class UMapGrid2D;
class UCellActorPlacementSettings;

/** Places arbitrary ACellActor subclasses in empty cells per zone. */
UCLASS()
class UCellActorPlacer : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Generation|Actors")
    bool Generate(UMapGrid2D* MapGrid, const UCellActorPlacementSettings* Settings, UWorld* World);

private:
    void CollectCandidatesForZone(UMapGrid2D* Map,
                                  int32 ZoneId,
                                  bool bOnlyInRooms,
                                  TArray<FIntPoint>& OutCandidates) const;
};

