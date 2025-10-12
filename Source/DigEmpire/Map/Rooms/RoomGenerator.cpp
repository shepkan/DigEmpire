#include "RoomGenerator.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "RoomTypes.h"

bool URoomGenerator::Generate(UMapGrid2D* MapGrid,
                              const TArray<int32>& ZoneLabels,
                              const URoomGenSettings* Settings,
                              const UZoneBorderSettings* BorderSettings)
{
    if (!MapGrid || !Settings || !BorderSettings) return false;
    const FIntPoint Size = MapGrid->GetSize();
    const int32 W = Size.X, H = Size.Y;
    if (W <= 0 || H <= 0) return false;
    if (ZoneLabels.Num() != W * H) return false;

    FRandomStream RNG(Settings->RandomSeed);
    if (Settings->RandomSeed < 0) { RNG.GenerateNewSeed(); }
    const int32 MaxAttempts = Settings->MaxPlacementAttempts;

    bool bAnyPlaced = false;
    for (const FRoomSpec& Spec : Settings->Rooms)
    {
        int32 TargetZone = Spec.ZoneId;

        // If auto-pick, choose the first zone that can fit this room.
        if (TargetZone < 0)
        {
            // Build list of zones [0..MaxLabel] and shuffle
            int32 MaxLabel = 0; for (int v : ZoneLabels) if (v > MaxLabel) MaxLabel = v;
            TArray<int32> Zones; Zones.Reserve(MaxLabel + 1);
            for (int32 z = 0; z <= MaxLabel; ++z) Zones.Add(z);
            for (int32 i = Zones.Num() - 1; i > 0; --i) { const int32 j = RNG.RandRange(0, i); Zones.Swap(i, j); }

            bool bPlaced = false;
            for (int32 z : Zones)
            {
                bPlaced = TryPlaceRoomInZone(MapGrid, Size, z, Spec.Width, Spec.Height, ZoneLabels, BorderSettings, MaxAttempts, RNG);
                if (bPlaced) { bAnyPlaced = true; break; }
            }
            continue; // move to next room spec
        }

        if (TryPlaceRoomInZone(MapGrid, Size, TargetZone, Spec.Width, Spec.Height, ZoneLabels, BorderSettings, MaxAttempts, RNG))
        {
            bAnyPlaced = true;
        }
    }

    return bAnyPlaced;
}

bool URoomGenerator::TryPlaceRoomInZone(UMapGrid2D* Map,
                                        const FIntPoint& Size,
                                        int32 ZoneId,
                                        int32 RoomW,
                                        int32 RoomH,
                                        const TArray<int32>& Labels,
                                        const UZoneBorderSettings* BorderSettings,
                                        int32 MaxAttempts,
                                        FRandomStream& RNG)
{
    const int32 W = Size.X, H = Size.Y;
    if (RoomW <= 0 || RoomH <= 0 || RoomW > W || RoomH > H) return false;
    auto TryAt = [&](int32 x0, int32 y0)->bool
    {
            bool bFits = true;
            for (int32 dy = 0; dy < RoomH && bFits; ++dy)
            {
                for (int32 dx = 0; dx < RoomW; ++dx)
                {
                    const int32 x = x0 + dx;
                    const int32 y = y0 + dy;
                    const int32 id = Idx(x, y, W);
                    if (Labels[id] != ZoneId)
                    {
                        bFits = false; break;
                    }
                    // Also ensure there is no wall/object currently.
                    int32 DummyDur = 0; FGameplayTag DummyTag;
                    if (Map->GetObjectAt(x, y, DummyTag, DummyDur))
                    {
                        bFits = false; break;
                    }
                }
            }
            if (!bFits) return false;

            // Ensure a 1-cell free corridor around the room: all outside-neighboring cells must be in-bounds and empty.
            if (!(x0 > 0 && y0 > 0 && (x0 + RoomW) < W && (y0 + RoomH) < H))
            {
                return false; // no clearance at map edge
            }

            auto IsFree = [&](int32 cx, int32 cy)->bool
            {
                FGameplayTag T; int32 D = 0;
                return !Map->GetObjectAt(cx, cy, T, D);
            };

            const int32 xOutMin = x0 - 1;
            const int32 xOutMax = x0 + RoomW;
            const int32 yOutMin = y0 - 1;
            const int32 yOutMax = y0 + RoomH;

            bool bClearRing = true;
            // Top and bottom outside rows (including corners)
            for (int32 x = xOutMin; x <= xOutMax && bClearRing; ++x)
            {
                if (!Map->IsInBounds(x, yOutMin) || !IsFree(x, yOutMin)) { bClearRing = false; break; }
                if (!Map->IsInBounds(x, yOutMax) || !IsFree(x, yOutMax)) { bClearRing = false; break; }
            }
            // Left and right outside columns
            for (int32 y = y0; y < y0 + RoomH && bClearRing; ++y)
            {
                if (!Map->IsInBounds(xOutMin, y) || !IsFree(xOutMin, y)) { bClearRing = false; break; }
                if (!Map->IsInBounds(xOutMax, y) || !IsFree(xOutMax, y)) { bClearRing = false; break; }
            }

            if (!bClearRing) return false;

            // Choose a single entrance on the room border such that outside cell is not a wall/object.
            int32 entranceX = -1, entranceY = -1;
            auto IsOutsideFree = [&](int32 ox, int32 oy) -> bool
            {
                if (!Map->IsInBounds(ox, oy)) return false;
                FGameplayTag T; int32 D = 0;
                return !Map->GetObjectAt(ox, oy, T, D);
            };

            // Build a list of candidate entrance cells along the border where the outside cell is free.
            TArray<FIntPoint> Candidates;
            // Top edge (north) — skip corners
            for (int32 dx = 1; dx < RoomW - 1; ++dx)
            {
                const int32 x = x0 + dx; const int32 y = y0;
                if (IsOutsideFree(x, y - 1)) Candidates.Add(FIntPoint(x, y));
            }
            // Bottom edge (south) — skip corners
            for (int32 dx = 1; dx < RoomW - 1; ++dx)
            {
                const int32 x = x0 + dx; const int32 y = y0 + RoomH - 1;
                if (IsOutsideFree(x, y + 1)) Candidates.Add(FIntPoint(x, y));
            }
            // Left edge (west) — skip corners
            for (int32 dy = 1; dy < RoomH - 1; ++dy)
            {
                const int32 x = x0; const int32 y = y0 + dy;
                if (IsOutsideFree(x - 1, y)) Candidates.Add(FIntPoint(x, y));
            }
            // Right edge (east) — skip corners
            for (int32 dy = 1; dy < RoomH - 1; ++dy)
            {
                const int32 x = x0 + RoomW - 1; const int32 y = y0 + dy;
                if (IsOutsideFree(x + 1, y)) Candidates.Add(FIntPoint(x, y));
            }

            if (Candidates.Num() > 0)
            {
                const int32 idx = RNG.RandRange(0, Candidates.Num() - 1);
                entranceX = Candidates[idx].X;
                entranceY = Candidates[idx].Y;
            }

            // If no valid entrance found, skip this placement.
            if (entranceX < 0) return false;

            // Build walls along the rectangle border, skipping the entrance cell.
            const FGameplayTag WallTag = BorderSettings->WallObjectTag;
            const int32 WallHP = BorderSettings->WallDurability;

            // Top and bottom rows
            for (int32 dx = 0; dx < RoomW; ++dx)
            {
                const int32 xt = x0 + dx;
                const int32 yt = y0;
                const int32 xb = x0 + dx;
                const int32 yb = y0 + RoomH - 1;
                // Top
                if (!(xt == entranceX && yt == entranceY))
                {
                    Map->AddOrUpdateObjectAt(xt, yt, WallTag, WallHP);
                }
                // Bottom
                if (!(xb == entranceX && yb == entranceY))
                {
                    Map->AddOrUpdateObjectAt(xb, yb, WallTag, WallHP);
                }
            }

            // Left and right columns (skip corners which were set above)
            for (int32 dy = 1; dy < RoomH - 1; ++dy)
            {
                const int32 xl = x0;
                const int32 yl = y0 + dy;
                const int32 xr = x0 + RoomW - 1;
                const int32 yr = y0 + dy;
                if (!(xl == entranceX && yl == entranceY))
                {
                    Map->AddOrUpdateObjectAt(xl, yl, WallTag, WallHP);
                }
                if (!(xr == entranceX && yr == entranceY))
                {
                    Map->AddOrUpdateObjectAt(xr, yr, WallTag, WallHP);
                }
            }

            // Success placing this room: record room info on the map
            FRoomInfo Info;
            Info.ZoneId = ZoneId;
            Info.TopLeft = FIntPoint(x0, y0);
            Info.Size = FIntPoint(RoomW, RoomH);
            Info.Entrance = FIntPoint(entranceX, entranceY);
            Map->AddRoom(Info);
            return true;
    };

    if (MaxAttempts > 0)
    {
        for (int32 attempt = 0; attempt < MaxAttempts; ++attempt)
        {
            const int32 x0 = RNG.RandRange(0, W - RoomW);
            const int32 y0 = RNG.RandRange(0, H - RoomH);
            if (TryAt(x0, y0)) return true;
        }
        return false;
    }
    else
    {
        // Deterministic full scan
        for (int32 y0 = 0; y0 <= H - RoomH; ++y0)
        {
            for (int32 x0 = 0; x0 <= W - RoomW; ++x0)
            {
                if (TryAt(x0, y0)) return true;
            }
        }
        return false;
    }
}
