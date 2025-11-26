#include "CellActorPlacer.h"
#include "CellActorPlacementSettings.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "Engine/World.h"
#include "DigEmpire/Map/CellActor.h"

bool UCellActorPlacer::Generate(UMapGrid2D* MapGrid, const UCellActorPlacementSettings* Settings, UWorld* World)
{
    if (!MapGrid || !Settings || !World) return false;
    if (Settings->Placements.Num() == 0) return true;

    // RNG
    FRandomStream RNG(Settings->RandomSeed);
    if (Settings->RandomSeed < 0) RNG.GenerateNewSeed();

    const FIntPoint Size = MapGrid->GetSize();
    if (Size.X <= 0 || Size.Y <= 0) return false;

    for (const FZoneActorPlacement& P : Settings->Placements)
    {
        if (!P.ActorClass) continue;
        if (P.CountPerZone <= 0) continue;
        for (int32 ZoneId : P.Zones)
        {
            if (ZoneId < 0) continue;

            TArray<FIntPoint> Candidates;
            CollectCandidatesForZone(MapGrid, ZoneId, P.bOnlyInRooms, Candidates);
            if (Candidates.Num() == 0) continue;

            // Shuffle candidates
            for (int32 i = Candidates.Num() - 1; i > 0; --i)
            {
                const int32 j = RNG.RandRange(0, i);
                if (i != j) Swap(Candidates[i], Candidates[j]);
            }

            const int32 ToPlace = FMath::Min(P.CountPerZone, Candidates.Num());
            int32 Placed = 0;
            for (int32 idx = 0; idx < Candidates.Num() && Placed < ToPlace; ++idx)
            {
                const FIntPoint C = Candidates[idx];
                if (MapGrid->GetActorAt(C.X, C.Y))
                {
                    continue; // became occupied
                }
                FGameplayTag Obj; int32 Dur;
                if (MapGrid->GetObjectAt(C.X, C.Y, Obj, Dur))
                {
                    continue; // not empty
                }
                const FVector SpawnLoc(C.X * Settings->TileSizeUU,
                                       C.Y * Settings->TileSizeUU,
                                       Settings->ZOffsetUU);
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                AActor* Spawned = World->SpawnActor<AActor>(P.ActorClass, SpawnLoc, FRotator::ZeroRotator, Params);
                if (ACellActor* CellAct = Cast<ACellActor>(Spawned))
                {
                    MapGrid->SetActorAt(C.X, C.Y, CellAct);
                    ++Placed;
                }
            }
        }
    }
    return true;
}

void UCellActorPlacer::CollectCandidatesForZone(UMapGrid2D* Map,
                                                int32 ZoneId,
                                                bool bOnlyInRooms,
                                                TArray<FIntPoint>& OutCandidates) const
{
    OutCandidates.Reset();
    if (!Map || ZoneId < 0) return;

    const FIntPoint Size = Map->GetSize();

    if (bOnlyInRooms)
    {
        TArray<FRoomInfo> Rooms = Map->GetRoomsForZone(ZoneId);
        for (const FRoomInfo& R : Rooms)
        {
            const int32 X0 = R.TopLeft.X;
            const int32 Y0 = R.TopLeft.Y;
            const int32 X1 = X0 + R.Size.X - 1;
            const int32 Y1 = Y0 + R.Size.Y - 1;
            for (int32 y = Y0; y <= Y1; ++y)
            {
                for (int32 x = X0; x <= X1; ++x)
                {
                    if (!Map->IsInBounds(x, y)) continue;
                    if (Map->GetZoneAt(x, y) != ZoneId) continue;
                    if (Map->GetActorAt(x, y)) continue;
                    FGameplayTag Obj; int32 Dur;
                    if (Map->GetObjectAt(x, y, Obj, Dur)) continue; // must be empty
                    OutCandidates.Add(FIntPoint(x, y));
                }
            }
        }
    }
    else
    {
        // Any cell in zone that is NOT inside any room rect
        // First, mark room cells in a set for quick rejection
        TSet<int32> RoomCells;
        RoomCells.Reserve(256);
        auto ToIndex = [&](int32 X, int32 Y){ return X + Y * Size.X; };
        TArray<FRoomInfo> Rooms = Map->GetRoomsForZone(ZoneId);
        for (const FRoomInfo& R : Rooms)
        {
            const int32 X0 = R.TopLeft.X;
            const int32 Y0 = R.TopLeft.Y;
            const int32 X1 = X0 + R.Size.X - 1;
            const int32 Y1 = Y0 + R.Size.Y - 1;
            for (int32 y = Y0; y <= Y1; ++y)
            {
                for (int32 x = X0; x <= X1; ++x)
                {
                    if (!Map->IsInBounds(x, y)) continue;
                    if (Map->GetZoneAt(x, y) != ZoneId) continue;
                    RoomCells.Add(ToIndex(x, y));
                }
            }
        }

        for (int32 y = 0; y < Size.Y; ++y)
        {
            for (int32 x = 0; x < Size.X; ++x)
            {
                if (Map->GetZoneAt(x, y) != ZoneId) continue;
                if (RoomCells.Contains(ToIndex(x, y))) continue;
                if (Map->GetActorAt(x, y)) continue;
                FGameplayTag Obj; int32 Dur;
                if (Map->GetObjectAt(x, y, Obj, Dur)) continue; // must be empty
                OutCandidates.Add(FIntPoint(x, y));
            }
        }
    }
}
