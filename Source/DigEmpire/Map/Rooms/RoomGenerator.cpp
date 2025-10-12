#include "RoomGenerator.h"
#include "DigEmpire/Map/MapGrid2D.h"

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

    bool bAnyPlaced = false;
    for (const FRoomSpec& Spec : Settings->Rooms)
    {
        int32 TargetZone = Spec.ZoneId;

        // If auto-pick, choose the first zone that can fit this room.
        if (TargetZone < 0)
        {
            // Naive search across zones 0..MaxLabel
            int32 MaxLabel = 0;
            for (int v : ZoneLabels) if (v > MaxLabel) MaxLabel = v;
            bool bPlaced = false;
            for (int32 z = 0; z <= MaxLabel && !bPlaced; ++z)
            {
                bPlaced = TryPlaceRoomInZone(MapGrid, Size, z, Spec.Width, Spec.Height, ZoneLabels, BorderSettings);
                if (bPlaced) { bAnyPlaced = true; }
            }
            continue; // move to next room spec
        }

        if (TryPlaceRoomInZone(MapGrid, Size, TargetZone, Spec.Width, Spec.Height, ZoneLabels, BorderSettings))
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
                                        const UZoneBorderSettings* BorderSettings)
{
    const int32 W = Size.X, H = Size.Y;
    if (RoomW <= 0 || RoomH <= 0 || RoomW > W || RoomH > H) return false;

    // Simple scan for a top-left that fits fully inside the given zone and has no blocking objects.
    for (int32 y0 = 0; y0 <= H - RoomH; ++y0)
    {
        for (int32 x0 = 0; x0 <= W - RoomW; ++x0)
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
            if (!bFits) continue;

            // Choose a single entrance on the room border: pick middle of one side (prefer south side).
            const int32 ex = x0 + RoomW / 2;
            const int32 ey = y0 + RoomH; // outside to the south
            // Entrance cell on the border we leave open (south edge center)
            const int32 entranceX = x0 + RoomW / 2;
            const int32 entranceY = y0 + RoomH - 1; // south edge cell inside the room border

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

            // Success placing this room
            return true;
        }
    }

    return false;
}

