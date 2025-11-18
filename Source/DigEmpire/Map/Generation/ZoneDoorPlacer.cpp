#include "ZoneDoorPlacer.h"
#include "ZoneDoorSettings.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/DoorCellActor.h"
#include "DigEmpire/Map/KeyCellActor.h"
#include "Engine/World.h"

bool UZoneDoorPlacer::Generate(UMapGrid2D* MapGrid, const UZoneDoorSettings* Settings, UWorld* World)
{
    if (!MapGrid || !Settings || !World) return false;
    if (!Settings->DoorClass) return false;

    const TArray<FZonePassage>& Passages = MapGrid->GetPassages();
    if (Passages.Num() == 0) return true; // nothing to do

    struct FPlacedDoor { ADoorCellActor* Door = nullptr; int32 ZoneA = -1; int32 ZoneB = -1; FIntPoint Cell; };
    TArray<FPlacedDoor> Placed;
    Placed.Reserve(Passages.Num());

    // 1) Place doors at passages and collect them
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
        FPlacedDoor D;
        D.Door = Cast<ADoorCellActor>(Spawned);
        D.ZoneA = P.ZoneA;
        D.ZoneB = P.ZoneB;
        D.Cell = Mid;
        Placed.Add(D);
    }

    // 2) Assign door colors and spawn keys per zone using depth order rules
    UMapGrid2DComponent* Comp = Cast<UMapGrid2DComponent>(MapGrid->GetOuter());
    if (!Comp)
    {
        return true; // doors placed; can't compute depth-based keys
    }

    // Determine zones present (from passages)
    TSet<int32> ZonesSet;
    for (const FZonePassage& P : Passages) { ZonesSet.Add(P.ZoneA); ZonesSet.Add(P.ZoneB); }
    TArray<int32> Zones = ZonesSet.Array();
    Zones.Sort([&](int32 A, int32 B)
    {
        const int32 dA = Comp->GetZoneDepth(A);
        const int32 dB = Comp->GetZoneDepth(B);
        if (dA != dB) return dA < dB;
        return A < B;
    });

    TSet<int32> ColoredDoorIdx; // indices in Placed already colored
    for (int32 ZoneId : Zones)
    {
        const int32 ZoneDepth = Comp->GetZoneDepth(ZoneId);
        if (ZoneDepth < 0) continue;

        // Fetch color tag for this zone
        const FGameplayTag* ColorPtr = Settings->ZoneColorTags.Find(ZoneId);
        if (!ColorPtr || !ColorPtr->IsValid())
        {
            continue; // cannot color or spawn keys without a valid tag
        }
        const FGameplayTag Color = *ColorPtr;

        // Claim doors for this zone
        bool bClaimedAny = false;
        for (int32 i = 0; i < Placed.Num(); ++i)
        {
            if (ColoredDoorIdx.Contains(i)) continue;
            FPlacedDoor& D = Placed[i];
            if (!D.Door) continue;
            const bool bAdjacent = (D.ZoneA == ZoneId || D.ZoneB == ZoneId);
            if (!bAdjacent) continue;

            const int32 Other = (D.ZoneA == ZoneId) ? D.ZoneB : D.ZoneA;
            const int32 OtherDepth = Comp->GetZoneDepth(Other);
            if (ZoneDepth == 0)
            {
                // Zone 0 claims all its adjacent doors
                D.Door->DoorColor = Color;
                ColoredDoorIdx.Add(i);
                bClaimedAny = true;
            }
            else
            {
                // Only claim doors that lead to zones with depth != 0, and are still uncolored
                if (OtherDepth != 0)
                {
                    D.Door->DoorColor = Color;
                    ColoredDoorIdx.Add(i);
                    bClaimedAny = true;
                }
            }
        }

        // Spawn a single key in this zone if claimed any doors
        if (bClaimedAny && Settings->KeyClass)
        {
            FIntPoint KeyCell;
            if (FindFreeCellInZone(MapGrid, ZoneId, KeyCell))
            {
                const FVector KLoc(KeyCell.X * Settings->TileSizeUU,
                                   KeyCell.Y * Settings->TileSizeUU,
                                   Settings->ZOffsetUU);
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                AActor* SpawnedKey = World->SpawnActor<AActor>(Settings->KeyClass, KLoc, FRotator::ZeroRotator, Params);
                if (ACellActor* CellAct = Cast<ACellActor>(SpawnedKey))
                {
                    MapGrid->SetActorAt(KeyCell.X, KeyCell.Y, CellAct);
                }
                if (AKeyCellActor* Key = Cast<AKeyCellActor>(SpawnedKey))
                {
                    Key->DoorColor = Color;
                }
            }
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

bool UZoneDoorPlacer::FindFreeCellInZone(UMapGrid2D* Map, int32 ZoneId, /*out*/ FIntPoint& OutCell) const
{
    if (!Map || ZoneId < 0) return false;
    const FIntPoint Size = Map->GetSize();
    for (int32 y = 0; y < Size.Y; ++y)
    {
        for (int32 x = 0; x < Size.X; ++x)
        {
            if (Map->GetZoneAt(x, y) != ZoneId) continue;
            if (Map->GetActorAt(x, y)) continue;
            FGameplayTag T; int32 D;
            if (Map->GetObjectAt(x, y, T, D)) continue; // occupied by wall/object
            OutCell = FIntPoint(x, y);
            return true;
        }
    }
    return false;
}
