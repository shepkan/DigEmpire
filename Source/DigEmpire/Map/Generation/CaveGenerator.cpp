#include "CaveGenerator.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UCaveGenerator::Generate(UMapGrid2D* MapGrid,
                              const TArray<int32>& ZoneLabels,
                              const UCaveGenSettings* Settings,
                              const UZoneBorderSettings* BorderSettings)
{
    if (!MapGrid || !Settings || !BorderSettings) return false;
    const FIntPoint Size = MapGrid->GetSize();
    const int32 W = Size.X, H = Size.Y;
    if (W <= 0 || H <= 0) return false;
    if (ZoneLabels.Num() != W * H) return false;

    FRandomStream RNG(Settings->RandomSeed);
    if (Settings->RandomSeed < 0) RNG.GenerateNewSeed();

    // Determine max zone id
    int32 MaxZoneId = 0; for (int v : ZoneLabels) if (v > MaxZoneId) MaxZoneId = v;

    TArray<int8> FixedState; FixedState.SetNum(W * H);
    TArray<uint8> Cur; Cur.SetNumZeroed(W * H);
    TArray<uint8> Nxt; Nxt.SetNumZeroed(W * H);

    for (int32 ZoneId = 0; ZoneId <= MaxZoneId; ++ZoneId)
    {
        // Build masks and initial state for this zone
        BuildZoneMasks(MapGrid, ZoneLabels, ZoneId, BorderSettings, FixedState, Cur);

        // Randomize mutable cells using FillChance
        for (int32 y = 0; y < H; ++y)
        {
            for (int32 x = 0; x < W; ++x)
            {
                const int32 id = Idx(x, y, W);
                if (ZoneLabels[id] != ZoneId) continue;
                if (FixedState[id] == -1)
                {
                    Cur[id] = (RNG.FRand() < Settings->FillChance) ? 1 : 0;
                }
                else
                {
                    Cur[id] = (FixedState[id] == 1) ? 1 : 0;
                }
            }
        }

        // Iterations
        for (int iter = 0; iter < Settings->Iterations; ++iter)
        {
            for (int32 y = 0; y < H; ++y)
            for (int32 x = 0; x < W; ++x)
            {
                const int32 id = Idx(x, y, W);
                if (ZoneLabels[id] != ZoneId) continue;

                if (FixedState[id] != -1)
                {
                    Nxt[id] = (FixedState[id] == 1) ? 1 : 0; // immutable
                    continue;
                }

                const int n = CountNeighbors8(FixedState, Cur, ZoneLabels, ZoneId, x, y, W, H);
                if (Cur[id] == 1)
                {
                    Nxt[id] = (n >= Settings->SurvivalLimit) ? 1 : 0;
                }
                else
                {
                    Nxt[id] = (n >= Settings->BirthLimit) ? 1 : 0;
                }
            }
            // swap buffers
            Swap(Cur, Nxt);
        }

        // Apply back to map for mutable cells only
        for (int32 y = 0; y < H; ++y)
        for (int32 x = 0; x < W; ++x)
        {
            const int32 id = Idx(x, y, W);
            if (ZoneLabels[id] != ZoneId) continue;
            if (FixedState[id] != -1) continue; // leave immutable

            if (Cur[id] == 1)
            {
                MapGrid->AddOrUpdateObjectAt(x, y, BorderSettings->WallObjectTag, BorderSettings->WallDurability);
            }
            else
            {
                MapGrid->RemoveObjectAt(x, y);
            }
        }
    }

    return true;
}

void UCaveGenerator::BuildZoneMasks(UMapGrid2D* Map,
                                    const TArray<int32>& Labels,
                                    int32 ZoneId,
                                    const UZoneBorderSettings* BorderSettings,
                                    TArray<int8>& FixedState,
                                    TArray<uint8>& Cur)
{
    const FIntPoint Size = Map->GetSize();
    const int32 W = Size.X, H = Size.Y;
    const TArray<FRoomInfo>& Rooms = Map->GetRooms();
    const TArray<FZonePassage>& Passages = Map->GetPassages();

    // Precompute quick lookup for rooms and passages (immutable empty)
    TSet<FIntPoint> ImmutableEmpty;
    RoomWallCells.Reset();
    for (const FRoomInfo& R : Rooms)
    {
        if (R.ZoneId != ZoneId) continue;
        for (int32 dy = 0; dy < R.Size.Y; ++dy)
        for (int32 dx = 0; dx < R.Size.X; ++dx)
        {
            ImmutableEmpty.Add(FIntPoint(R.TopLeft.X + dx, R.TopLeft.Y + dy));
        }

        // Collect actual wall cells placed around the room (exclude entrance left open)
        const int32 x0 = R.TopLeft.X;
        const int32 y0 = R.TopLeft.Y;
        const int32 w = R.Size.X;
        const int32 h = R.Size.Y;
        // Top and bottom edges
        for (int32 dx = 0; dx < w; ++dx)
        {
            const int32 xt = x0 + dx;
            const int32 yt = y0;
            const int32 xb = x0 + dx;
            const int32 yb = y0 + h - 1;
            FGameplayTag T; int32 D;
            if (Map->GetObjectAt(xt, yt, T, D) && T == BorderSettings->WallObjectTag) RoomWallCells.Add(FIntPoint(xt, yt));
            if (Map->GetObjectAt(xb, yb, T, D) && T == BorderSettings->WallObjectTag) RoomWallCells.Add(FIntPoint(xb, yb));
        }
        // Left and right edges (skip corners)
        for (int32 dy2 = 1; dy2 < h - 1; ++dy2)
        {
            const int32 xl = x0;
            const int32 yl = y0 + dy2;
            const int32 xr = x0 + w - 1;
            const int32 yr = y0 + dy2;
            FGameplayTag T; int32 D;
            if (Map->GetObjectAt(xl, yl, T, D) && T == BorderSettings->WallObjectTag) RoomWallCells.Add(FIntPoint(xl, yl));
            if (Map->GetObjectAt(xr, yr, T, D) && T == BorderSettings->WallObjectTag) RoomWallCells.Add(FIntPoint(xr, yr));
        }
    }
    for (const FZonePassage& P : Passages)
    {
        // Passages connect two zones; we treat carved cells as immutable empty for both
        for (const FIntPoint& C : P.Cells)
        {
            ImmutableEmpty.Add(C);
        }
    }

    // Fill masks
    for (int32 y = 0; y < H; ++y)
    for (int32 x = 0; x < W; ++x)
    {
        const int32 id = Idx(x, y, W);
        if (Labels[id] != ZoneId)
        {
            FixedState[id] = 1; // treat outside zone as wall for counting
            Cur[id] = 1;
            continue;
        }

        // Room interior or passage cells are immutable empty
        if (ImmutableEmpty.Contains(FIntPoint(x, y)))
        {
            FixedState[id] = 0;
            Cur[id] = 0;
            continue;
        }

        // Check if there is a wall object here
        FGameplayTag ObjTag; int32 Dur = 0;
        const bool bHasObj = Map->GetObjectAt(x, y, ObjTag, Dur);
        if (bHasObj && ObjTag == BorderSettings->WallObjectTag)
        {
            FixedState[id] = 1; // immutable wall
            Cur[id] = 1;
        }
        else
        {
            FixedState[id] = -1; // mutable cell
            Cur[id] = 0;
        }
    }
}

int32 UCaveGenerator::CountNeighbors8(const TArray<int8>& Fixed,
                                      const TArray<uint8>& Cur,
                                      const TArray<int32>& Labels,
                                      int32 ZoneId,
                                      int32 X, int32 Y,
                                      int32 W, int32 H) const
{
    int32 Count = 0;
    for (int dy = -1; dy <= 1; ++dy)
    for (int dx = -1; dx <= 1; ++dx)
    {
        if (dx == 0 && dy == 0) continue;
        const int nx = X + dx, ny = Y + dy;
        if (nx < 0 || ny < 0 || nx >= W || ny >= H)
        {
            ++Count; // treat out of map as wall
            continue;
        }
        const int id = Idx(nx, ny, W);
        if (Labels[id] != ZoneId)
        {
            ++Count; // outside zone = wall
            continue;
        }
        // Within zone: fixed overrides current
        if (Fixed[id] == -1)
        {
            if (Cur[id] == 1) ++Count;
        }
        else
        {
            if (Fixed[id] == 1)
            {
                // Immutable wall counts as a wall; if it's a room wall, add extra weight
                ++Count;
                if (RoomWallCells.Contains(FIntPoint(nx, ny))) ++Count; // bias near room walls
            }
        }
    }
    return Count;
}
