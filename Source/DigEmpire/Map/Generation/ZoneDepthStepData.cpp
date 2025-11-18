#include "ZoneDepthStepData.h"

#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "ZonePassageTypes.h"

void UZoneDepthStepData::ExecuteGenerationStep(UMapGrid2D* Map, UWorld* /*World*/, TArray<int32>& InOutZoneLabels) const
{
    if (!Map) return;
    if (InOutZoneLabels.Num() == 0) return;

    // Determine number of zones
    int32 MaxZoneId = 0; for (int v : InOutZoneLabels) if (v > MaxZoneId) MaxZoneId = v;
    const int32 NumZones = MaxZoneId + 1;
    if (NumZones <= 0) return;

    // Build adjacency from passages
    TArray<TSet<int32>> Adj; Adj.SetNum(NumZones);
    const TArray<FZonePassage>& Passages = Map->GetPassages();
    for (const FZonePassage& P : Passages)
    {
        if (P.ZoneA >= 0 && P.ZoneA < NumZones && P.ZoneB >= 0 && P.ZoneB < NumZones)
        {
            Adj[P.ZoneA].Add(P.ZoneB);
            Adj[P.ZoneB].Add(P.ZoneA);
        }
    }

    // BFS from zone 0 to compute minimal hop depths
    TArray<int32> Depths; Depths.Init(-1, NumZones);
    TQueue<int32> Q;
    if (NumZones > 0)
    {
        Depths[0] = 0;
        Q.Enqueue(0);
    }
    int32 cur;
    while (Q.Dequeue(cur))
    {
        const int32 d = Depths[cur];
        for (int32 nb : Adj[cur])
        {
            if (Depths[nb] == -1)
            {
                Depths[nb] = d + 1;
                Q.Enqueue(nb);
            }
        }
    }

    if (UObject* OuterObj = Map->GetOuter())
    {
        if (UMapGrid2DComponent* Comp = Cast<UMapGrid2DComponent>(OuterObj))
        {
            Comp->SetZoneDepths(Depths);
        }
    }
}

