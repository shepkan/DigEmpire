#include "OreGenerator.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UOreGenerator::Generate(UMapGrid2D* MapGrid,
                             const TArray<int32>& ZoneLabels,
                             const UOreGenSettings* Settings)
{
    if (!MapGrid || !Settings) return false;
    const FIntPoint Size = MapGrid->GetSize();
    const int32 W = Size.X, H = Size.Y;
    if (W <= 0 || H <= 0) return false;
    if (ZoneLabels.Num() != W * H) return false;
    // Build map ZoneIndex -> zone config (list of ores)
    TMap<int32, const FZoneOreConfig*> ZoneIndexToConfig;
    ZoneIndexToConfig.Reserve(Settings->ZoneOres.Num());
    for (const FZoneOreConfig& Cfg : Settings->ZoneOres)
    {
        ZoneIndexToConfig.Add(Cfg.ZoneIndex, &Cfg);
    }

    // Determine max zone id
    int32 MaxZoneId = 0;
    for (int v : ZoneLabels) if (v > MaxZoneId) MaxZoneId = v;

    // RNG
    FRandomStream RNG(Settings->RandomSeed);
    if (Settings->RandomSeed < 0) RNG.GenerateNewSeed();

    // For each zone, collect candidate cells (object-occupied) and place ores
    for (int32 ZoneId = 0; ZoneId <= MaxZoneId; ++ZoneId)
    {
        const FZoneOreConfig* ZoneCfg = ZoneIndexToConfig.FindRef(ZoneId);
        if (!ZoneCfg || ZoneCfg->Ores.Num() == 0) continue; // unspecified -> none

        TArray<FIntPoint> Candidates;
        Candidates.Reserve(128);
        // Collect tiles that are occupied by a block (object-level), ignore actors
        for (int32 y = 0; y < H; ++y)
        for (int32 x = 0; x < W; ++x)
        {
            const int32 id = Idx(x, y, W);
            if (ZoneLabels[id] != ZoneId) continue;
            FGameplayTag Obj; int32 Dur = 0;
            if (MapGrid->GetObjectAt(x, y, Obj, Dur))
            {
                // Skip blocks explicitly forbidden for ore placement
                if (Settings->ForbiddenObjectTags.Contains(Obj))
                {
                    continue;
                }
                // Has an object with durability => treat as a solid block
                Candidates.Add(FIntPoint(x, y));
            }
        }

        if (Candidates.Num() == 0) continue;

        // Shuffle candidate indices once; use sequentially for different ores to avoid overlap
        TArray<int32> Idx;
        Idx.SetNumUninitialized(Candidates.Num());
        for (int32 i = 0; i < Candidates.Num(); ++i) Idx[i] = i;
        for (int32 i = Idx.Num() - 1; i > 0; --i)
        {
            const int32 j = RNG.RandRange(0, i);
            if (i != j) Swap(Idx[i], Idx[j]);
        }
        int32 Cursor = 0;

        for (const FOreCountRange& Ore : ZoneCfg->Ores)
        {
            if (!Ore.OreTag.IsValid()) continue;
            const int32 MinC = FMath::Max(0, Ore.MinCount);
            const int32 MaxC = FMath::Max(MinC, Ore.MaxCount);
            const int32 Remaining = Idx.Num() - Cursor;
            if (Remaining <= 0) break;
            const int32 Desired = FMath::Clamp(RNG.RandRange(MinC, MaxC), 0, Remaining);
            for (int32 k = 0; k < Desired; ++k)
            {
                const FIntPoint& C = Candidates[Idx[Cursor++]];
                MapGrid->SetOreAt(C.X, C.Y, Ore.OreTag);
            }
        }
    }

    return true;
}
