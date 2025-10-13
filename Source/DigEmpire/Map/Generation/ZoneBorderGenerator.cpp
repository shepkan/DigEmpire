#include "ZoneBorderGenerator.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UZoneBorderGenerator::Generate(UMapGrid2D* MapGrid,
                                    const TArray<int32>& ZoneLabels,
                                    const UZoneBorderSettings* Settings)
{
    if (!ValidateInputs(MapGrid, ZoneLabels, Settings))
        return false;

    CachedLabels = ZoneLabels;
    CachedSize = MapGrid->GetSize();

    // Collect adjacent zone boundary seeds on the lower-id (A) and higher-id (B) sides
    TMap<FIntPoint, TSet<FIntPoint>> PairToA, PairToB;
    CollectZoneBoundaries(CachedLabels, PairToA, PairToB);

    // Place walls along boundaries with configured thickness
    PlaceWallsWithThickness(MapGrid, Settings, PairToA);

    // Also place walls along the outer border of the map
    {
        const FIntPoint S = CachedSize;
        const int32 W = S.X, H = S.Y;
        if (W > 0 && H > 0)
        {
            // Top and bottom rows
            for (int32 x = 0; x < W; ++x)
            {
                PutWall(MapGrid, x, 0, Settings);
                PutWall(MapGrid, x, H - 1, Settings);
            }
            // Left and right columns
            for (int32 y = 0; y < H; ++y)
            {
                PutWall(MapGrid, 0, y, Settings);
                PutWall(MapGrid, W - 1, y, Settings);
            }
        }
    }
    return true;
}

TArray<FIntPoint> UZoneBorderGenerator::GetFreeCellsForZone(UMapGrid2D* MapGrid, int32 ZoneId) const
{
    TArray<FIntPoint> Out;
    if (!MapGrid || CachedLabels.Num() != CachedSize.X * CachedSize.Y) return Out;
    if (ZoneId < 0) return Out;

    const int32 W = CachedSize.X, H = CachedSize.Y;
    for (int32 y = 0; y < H; ++y)
    for (int32 x = 0; x < W; ++x)
    {
        const int32 id = Idx(x,y,W);
        if (CachedLabels[id] != ZoneId) continue;

        FGameplayTag Obj; int32 Durability = 0;
        const bool bHasObj = MapGrid->GetObjectAt(x, y, Obj, Durability);
        if (bHasObj && Obj.IsValid() && Durability > 0) continue;
        Out.Add(FIntPoint(x,y));
    }
    return Out;
}

bool UZoneBorderGenerator::ValidateInputs(const UMapGrid2D* Map,
                                          const TArray<int32>& Labels,
                                          const UZoneBorderSettings* Settings) const
{
    if (!Map || !Settings) return false;
    const FIntPoint S = Map->GetSize();
    if (S.X <= 0 || S.Y <= 0) return false;
    if (Labels.Num() != S.X * S.Y) return false;
    if (!Settings->WallObjectTag.IsValid() || Settings->WallDurability <= 0) return false;
    if (Settings->BorderThickness < 1 || Settings->PassageWidth < 1) return false;
    return true;
}

void UZoneBorderGenerator::CollectZoneBoundaries(const TArray<int32>& Labels,
                                                 TMap<FIntPoint, TSet<FIntPoint>>& OutPairToA,
                                                 TMap<FIntPoint, TSet<FIntPoint>>& OutPairToB) const
{
    const int32 W = CachedSize.X, H = CachedSize.Y;
    auto PairKey = [](int32 A, int32 B)->FIntPoint { return (A < B) ? FIntPoint(A,B) : FIntPoint(B,A); };

    for (int32 y = 0; y < H; ++y)
    for (int32 x = 0; x < W; ++x)
    {
        const int32 z = Labels[Idx(x,y,W)];
        if (z < 0) continue;
        // Right neighbor
        if (x < W - 1)
        {
            const int32 z2 = Labels[Idx(x+1,y,W)];
            if (z2 >= 0 && z2 != z)
            {
                const FIntPoint Key = PairKey(z, z2);
                if (Key.X == z)  { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x,   y)); }
                else             { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x+1, y)); }
                if (Key.Y == z)  { OutPairToB.FindOrAdd(Key).Add(FIntPoint(x,   y)); }
                else             { OutPairToB.FindOrAdd(Key).Add(FIntPoint(x+1, y)); }
            }
        }
        // Up neighbor
        if (y < H - 1)
        {
            const int32 z2 = Labels[Idx(x,y+1,W)];
            if (z2 >= 0 && z2 != z)
            {
                const FIntPoint Key = PairKey(z, z2);
                if (Key.X == z)  { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x, y  )); }
                else             { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x, y+1)); }
                if (Key.Y == z)  { OutPairToB.FindOrAdd(Key).Add(FIntPoint(x, y  )); }
                else             { OutPairToB.FindOrAdd(Key).Add(FIntPoint(x, y+1)); }
            }
        }
    }
}

void UZoneBorderGenerator::PlaceWallsWithThickness(UMapGrid2D* Map,
                                                   const UZoneBorderSettings* Settings,
                                                   const TMap<FIntPoint, TSet<FIntPoint>>& PairToA)
{
    for (const auto& kv : PairToA)
    {
        const FIntPoint Key = kv.Key; // (ZoneA, ZoneB)
        const TSet<FIntPoint>& ASeeds = kv.Value;

        TSet<FIntPoint> AExpanded;
        ExpandInwardWithinZone(ASeeds, Key.X, Settings->BorderThickness - 1, AExpanded);

        TSet<FIntPoint> ToWall = ASeeds;
        for (const FIntPoint& p : AExpanded) ToWall.Add(p);

        for (const FIntPoint& p : ToWall)
        {
            PutWall(Map, p.X, p.Y, Settings);
        }
    }
}

void UZoneBorderGenerator::ExpandInwardWithinZone(const TSet<FIntPoint>& Seeds,
                                                  int32 ZoneId,
                                                  int32 Rings,
                                                  TSet<FIntPoint>& OutExpanded) const
{
    if (Rings <= 0 || Seeds.Num() == 0) return;

    const int32 W = CachedSize.X, H = CachedSize.Y;
    TSet<FIntPoint> Current = Seeds;
    TSet<FIntPoint> Visited = Seeds;

    for (int32 r = 0; r < Rings; ++r)
    {
        TSet<FIntPoint> Next;
        for (const FIntPoint& c : Current)
        {
            const FIntPoint N4[4] = { {c.X+1,c.Y},{c.X-1,c.Y},{c.X,c.Y+1},{c.X,c.Y-1} };
            for (const FIntPoint& n : N4)
            {
                if (n.X < 0 || n.Y < 0 || n.X >= W || n.Y >= H) continue;
                if (Visited.Contains(n)) continue;

                const int32 lab = CachedLabels[Idx(n.X,n.Y,W)];
                if (lab != ZoneId) continue;              // stay inside the zone

                Visited.Add(n);
                Next.Add(n);
            }
        }
        for (const FIntPoint& v : Next) OutExpanded.Add(v);
        Current = MoveTemp(Next);
        if (Current.Num() == 0) break;
    }
}

void UZoneBorderGenerator::PutWall(UMapGrid2D* Map, int32 X, int32 Y, const UZoneBorderSettings* Settings) const
{
    Map->AddOrUpdateObjectAt(X, Y, Settings->WallObjectTag, Settings->WallDurability);
}
