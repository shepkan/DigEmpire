#include "ZoneDoorPlacer.h"
#include "ZoneDoorSettings.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "Engine/World.h"

bool UZoneDoorPlacer::Generate(UMapGrid2D* MapGrid, const UZoneDoorSettings* Settings, UWorld* World)
{
    if (!MapGrid || !Settings || !World) return false;
    if (!Settings->DoorClass) return false;

    const TArray<FZonePassage>& Passages = MapGrid->GetPassages();
    if (Passages.Num() == 0) return true; // nothing to do

    for (const FZonePassage& P : Passages)
    {
        if (P.Cells.Num() == 0) continue;
        const FIntPoint Mid = PickMidCell(P.Cells);

        // Avoid double placement if something already occupies the cell
        if (MapGrid->GetActorAt(Mid.X, Mid.Y))
        {
            continue;
        }

        const FVector SpawnLoc(Mid.X * Settings->TileSizeUU,
                               Mid.Y * Settings->TileSizeUU,
                               Settings->ZOffsetUU);
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        AActor* Spawned = World->SpawnActor<AActor>(Settings->DoorClass, SpawnLoc, FRotator::ZeroRotator, Params);
        if (ACellActor* CellAct = Cast<ACellActor>(Spawned))
        {
            MapGrid->SetActorAt(Mid.X, Mid.Y, CellAct);
        }
    }
    return true;
}

FIntPoint UZoneDoorPlacer::PickMidCell(const TArray<FIntPoint>& Cells)
{
    if (Cells.Num() == 0) return FIntPoint::ZeroValue;
    if (Cells.Num() == 1) return Cells[0];

    // Compute centroid and pick the closest cell by squared distance.
    double sumX = 0.0, sumY = 0.0;
    for (const FIntPoint& c : Cells) { sumX += c.X; sumY += c.Y; }
    const double cx = sumX / Cells.Num();
    const double cy = sumY / Cells.Num();

    int32 bestIdx = 0; double bestD2 = TNumericLimits<double>::Max();
    for (int32 i = 0; i < Cells.Num(); ++i)
    {
        const double dx = Cells[i].X - cx;
        const double dy = Cells[i].Y - cy;
        const double d2 = dx*dx + dy*dy;
        if (d2 < bestD2) { bestD2 = d2; bestIdx = i; }
    }
    return Cells[bestIdx];
}

