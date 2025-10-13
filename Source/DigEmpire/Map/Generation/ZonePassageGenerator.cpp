#include "ZonePassageGenerator.h"
#include "DrawDebugHelpers.h"
#include "Algo/RandomShuffle.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UZonePassageGenerator::Generate(UMapGrid2D* MapGrid,
                                     const TArray<int32>& ZoneLabels,
                                     const UZoneBorderSettings* Settings)
{
    if (!ValidateInputs(MapGrid, ZoneLabels, Settings))
        return false;

    CachedLabels = ZoneLabels;
    CachedSize = MapGrid->GetSize();
    PassageMask.Reset();
    Passages.Reset();

    TMap<FIntPoint, TSet<FIntPoint>> PairToA, PairToB;
    CollectZoneBoundaries(CachedLabels, PairToA, PairToB);
    ChooseAndCarvePassages(MapGrid, Settings, PairToA, PairToB);
    return true;
}

bool UZonePassageGenerator::ValidateInputs(const UMapGrid2D* Map,
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

void UZonePassageGenerator::CollectZoneBoundaries(const TArray<int32>& Labels,
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

void UZonePassageGenerator::ChooseAndCarvePassages(
    UMapGrid2D* Map,
    const UZoneBorderSettings* Settings,
    const TMap<FIntPoint, TSet<FIntPoint>>& PairToA,
    const TMap<FIntPoint, TSet<FIntPoint>>& PairToB)
{
    const int32 Seed = (Settings->RandomSeed >= 0) ? Settings->RandomSeed : FMath::Rand();
    FRandomStream RNG(Seed);

    PassageMask.Reset();
    Passages.Reset();

    // Degree caps
    TMap<int32,int32> DegreeCap, DegreeNow;
    TSet<int32> ZonesInvolved;
    for (const auto& kv : PairToA) { ZonesInvolved.Add(kv.Key.X); ZonesInvolved.Add(kv.Key.Y); }
    for (int32 z : ZonesInvolved) { DegreeNow.Add(z, 0); DegreeCap.Add(z, -1); }
    for (const FZonePassageCap& cap : Settings->DegreeCaps)
    {
        DegreeCap.FindOrAdd(cap.ZoneIndex) = cap.MaxPassages;
        if (!DegreeNow.Contains(cap.ZoneIndex)) DegreeNow.Add(cap.ZoneIndex, 0);
    }
    auto ZoneBelowCap = [&](int32 Z)->bool{
        const int32 cap = DegreeCap.Contains(Z) ? DegreeCap[Z] : -1;
        const int32 deg = DegreeNow.Contains(Z) ? DegreeNow[Z] : 0;
        return (cap < 0) || (deg < cap);
    };

    // Randomize pair order
    TArray<FIntPoint> Pairs; Pairs.Reserve(PairToA.Num());
    for (const auto& kv : PairToA) Pairs.Add(kv.Key);
    Algo::RandomShuffle(Pairs);

    const int32 W = CachedSize.X, H = CachedSize.Y;
    auto InBounds2 = [&](int x,int y){ return x>=0 && y>=0 && x<W && y<H; };

    for (const FIntPoint& Key : Pairs) // (ZoneA=min, ZoneB=max)
    {
        const TSet<FIntPoint>* ASetPtr = PairToA.Find(Key);
        const TSet<FIntPoint>* BSetPtr = PairToB.Find(Key);
        if (!ASetPtr || !BSetPtr || ASetPtr->Num()==0 || BSetPtr->Num()==0) continue;
        if (!ZoneBelowCap(Key.X) || !ZoneBelowCap(Key.Y)) continue;

        // Candidate anchors on A side
        TArray<FIntPoint> ACands;
        ACands.Reserve(ASetPtr->Num());
        for (const FIntPoint& c : *ASetPtr) ACands.Add(c);
        Algo::RandomShuffle(ACands);

        bool bCarved = false;
        int32 attemptsLeft = FMath::Max(1, Settings->AttemptsPerPair);

        // Try up to AttemptsPerPair different anchors
        for (int32 attempt = 0; attempt < attemptsLeft && !bCarved; ++attempt)
        {
            if (ACands.Num() == 0) break;
            const int32 pick = RNG.RandRange(0, ACands.Num()-1);
            const FIntPoint AnchorA = ACands[pick];
            ACands.RemoveAtSwap(pick);

            // Determine boundary orientation by checking B neighbors around the anchor
            bool bVertical=false; int dxToB=0, dyToB=0;
            if (BSetPtr->Contains(FIntPoint(AnchorA.X+1,AnchorA.Y))) { dxToB=+1; dyToB=0; bVertical=true; }
            else if (BSetPtr->Contains(FIntPoint(AnchorA.X-1,AnchorA.Y))) { dxToB=-1; dyToB=0; bVertical=true; }
            else if (BSetPtr->Contains(FIntPoint(AnchorA.X,AnchorA.Y+1))) { dxToB=0; dyToB=+1; bVertical=false; }
            else if (BSetPtr->Contains(FIntPoint(AnchorA.X,AnchorA.Y-1))) { dxToB=0; dyToB=-1; bVertical=false; }
            else
            {
                // Fallback: pick any B cell and infer direction
                const TSet<FIntPoint>& BSet = *BSetPtr;
                int32 j=0, idx = RNG.RandRange(0, BSet.Num()-1);
                FIntPoint anyB = AnchorA;
                for (const FIntPoint& b : BSet) { if (j++==idx){ anyB=b; break; } }
                dxToB = FMath::Clamp(anyB.X-AnchorA.X,-1,1);
                dyToB = FMath::Clamp(anyB.Y-AnchorA.Y,-1,1);
                bVertical = (dxToB!=0);
            }

            // Preview stripe to carve (no map modification yet)
            TArray<FIntPoint> ToClearA, ToClearB;
            if (!BuildCarveStripePreview(Map, AnchorA, bVertical, dxToB, dyToB,
                Settings->PassageWidth, Key.X, Key.Y, ToClearA, ToClearB))
            {
                continue; // invalid anchor: would exit bounds or hit wrong zones
            }

            // Spacing check (global)
            TArray<FIntPoint> Preview;
            Preview.Reserve(ToClearA.Num() + ToClearB.Num());
            Preview.Append(ToClearA); Preview.Append(ToClearB);
            if (IsTooCloseToExistingPassages(Preview, Settings->MinPassageDistance))
            {
                continue; // too close to existing passages
            }

            // Carve for real
            for (const FIntPoint& p : ToClearA) ClearCell(Map, p.X, p.Y);
            for (const FIntPoint& p : ToClearB) ClearCell(Map, p.X, p.Y);

            // Record passage
            FZonePassage Pass; 
            Pass.ZoneA = Key.X; Pass.ZoneB = Key.Y;
            Pass.Cells = MoveTemp(Preview);
            Passages.Add(MoveTemp(Pass));
            DegreeNow[Key.X]++; DegreeNow[Key.Y]++;

            // Update protection mask: passage cells + dilation to shield/space subsequent passages
            for (const FIntPoint& c : Passages.Last().Cells) PassageMask.Add(c);
            DilateMask(PassageMask, FMath::Max(0, Settings->BorderThickness - 1));
            DilateMask(PassageMask, Settings->MinPassageDistance);

            // Debug visualization by zone side
            if (Settings->bDebugDrawPassages)
            {
                TArray<FIntPoint> CellsA; CellsA.Reserve(Passages.Last().Cells.Num());
                TArray<FIntPoint> CellsB; CellsB.Reserve(Passages.Last().Cells.Num());
                for (const FIntPoint& c : Passages.Last().Cells)
                {
                    const int32 id = Idx(c.X, c.Y, CachedSize.X);
                    if (id >= 0 && id < CachedLabels.Num())
                    {
                        const int32 z = CachedLabels[id];
                        if (z == Key.X) CellsA.Add(c);
                        else if (z == Key.Y) CellsB.Add(c);
                    }
                }
                if (CellsA.Num() > 0) DebugDrawPassageCells(Settings, CellsA, FColor::Red, Map->GetWorld());
                if (CellsB.Num() > 0) DebugDrawPassageCells(Settings, CellsB, FColor::Blue, Map->GetWorld());
            }

            bCarved = true;
        }
    }
}

void UZonePassageGenerator::ClearCell(UMapGrid2D* Map, int32 X, int32 Y) const
{
    Map->RemoveObjectAt(X, Y);
}

void UZonePassageGenerator::DebugDrawPassageCells(const UZoneBorderSettings* Settings, const TArray<FIntPoint>& Cells, const FColor& Color, UWorld* World) const
{
    if (!Settings->bDebugDrawPassages || !World) return;
    for (const FIntPoint& c : Cells)
    {
        const FVector P(c.X * Settings->DebugTileSizeUU, c.Y * Settings->DebugTileSizeUU, Settings->DebugZOffset);
        FColor UseColor = Color;
        const int32 W = CachedSize.X;
        const int32 H = CachedSize.Y;
        if (W > 0 && H > 0 && CachedLabels.Num() == W * H && c.X >= 0 && c.Y >= 0 && c.X < W && c.Y < H)
        {
            const int32 id = Idx(c.X, c.Y, W);
            const int32 ZoneId = CachedLabels.IsValidIndex(id) ? CachedLabels[id] : -1;
            if (ZoneId >= 0)
            {
                const uint8 H8 = uint8((ZoneId * 47) & 0xFF);
                const FLinearColor Lin = FLinearColor::MakeFromHSV8(H8, 220, 255);
                UseColor = Lin.ToFColor(true);
            }
        }
        DrawDebugSphere(World, P, Settings->DebugSphereRadiusUU, 12, UseColor, Settings->DebugLifetime <= 0.f, Settings->DebugLifetime);
    }
}

void UZonePassageGenerator::DilateMask(TSet<FIntPoint>& InOutMask, int32 Radius) const
{
    if (Radius <= 0) return;
    TSet<FIntPoint> Add;
    for (const FIntPoint& p : InOutMask)
    {
        for (int32 dy = -Radius; dy <= Radius; ++dy)
        {
            const int32 rem = Radius - FMath::Abs(dy);
            for (int32 dx = -rem; dx <= rem; ++dx)
            {
                const FIntPoint q(p.X + dx, p.Y + dy);
                if (InBounds(q.X, q.Y)) Add.Add(q);
            }
        }
    }
    InOutMask.Append(Add);
}

bool UZonePassageGenerator::BuildCarveStripePreview(
    UMapGrid2D* Map,
    const FIntPoint& AnchorA,
    bool bVerticalBoundary,
    int dxToB, int dyToB,
    int32 PassageWidth,
    int32 ZoneA, int32 ZoneB,
    TArray<FIntPoint>& OutCellsA,
    TArray<FIntPoint>& OutCellsB) const
{
    OutCellsA.Reset();
    OutCellsB.Reset();

    const int32 W = CachedSize.X, H = CachedSize.Y;
    auto InBounds2 = [&](int x, int y){ return x>=0 && y>=0 && x<W && y<H; };
    auto IsEmpty = [&](int x, int y)->bool
    {
        FGameplayTag Obj; int32 Dur=0;
        if (!Map->GetObjectAt(x,y,Obj,Dur)) return true;
        return !(Obj.IsValid() && Dur > 0);
    };

    const int32 Half = FMath::Max(0, PassageWidth / 2);
    const FIntPoint Tangent  = bVerticalBoundary ? FIntPoint(0, 1) : FIntPoint(1, 0);
    const FIntPoint InwardA  = FIntPoint(-dxToB, -dyToB);  // into Zone A
    const FIntPoint OutwardB = FIntPoint(+dxToB, +dyToB);  // into Zone B

    TArray<FIntPoint> TerminalEmptyA;
    TArray<FIntPoint> TerminalEmptyB;

    for (int32 t = -Half; t <= +Half; ++t)
    {
        const int ax = AnchorA.X + Tangent.X * t;
        const int ay = AnchorA.Y + Tangent.Y * t;
        if (!InBounds2(ax, ay)) return false;

        int x=ax, y=ay;
        TArray<FIntPoint> ColA;
        {
            int steps=0; const int maxSteps = FMath::Max(CachedSize.X, CachedSize.Y);
            while (InBounds2(x, y) && steps++ < maxSteps)
            {
                if (CachedLabels[Idx(x,y,W)] != ZoneA) return false;
                if (IsEmpty(x,y)) { TerminalEmptyA.Add(FIntPoint(x,y)); break; }
                ColA.Add(FIntPoint(x,y));
                x += InwardA.X; y += InwardA.Y; 
            }
            if (!InBounds2(x,y)) return false;
        }

        x = ax + dxToB; y = ay + dyToB;
        TArray<FIntPoint> ColB;
        {
            int steps=0; const int maxSteps = FMath::Max(CachedSize.X, CachedSize.Y);
            bool bFoundEmptyB = false; FIntPoint TermB(0,0);
            while (InBounds2(x, y) && steps++ < maxSteps)
            {
                if (CachedLabels[Idx(x,y,W)] != ZoneB) return false;
                if (IsEmpty(x,y)) { TerminalEmptyB.Add(FIntPoint(x,y)); TermB = FIntPoint(x,y); bFoundEmptyB = true; break; }
                ColB.Add(FIntPoint(x,y));
                x += OutwardB.X; y += OutwardB.Y; 
            }
            if (!InBounds2(x,y)) return false;
            if (!bFoundEmptyB) return false;
            // Extend passage one more cell into the target zone: include terminal empty cell
            ColB.Add(TermB);
        }

        for (const FIntPoint& p : ColA) OutCellsA.Add(p);
        for (const FIntPoint& p : ColB) OutCellsB.Add(p);
    }

    for (const FIntPoint& e : TerminalEmptyA)
    {
        const FIntPoint n1(e.X + Tangent.X, e.Y + Tangent.Y);
        const FIntPoint n2(e.X - Tangent.X, e.Y - Tangent.Y);
        if (InBounds2(n1.X,n1.Y) && CachedLabels[Idx(n1.X,n1.Y,W)] == ZoneB) return false;
        if (InBounds2(n2.X,n2.Y) && CachedLabels[Idx(n2.X,n2.Y,W)] == ZoneB) return false;
    }
    for (const FIntPoint& e : TerminalEmptyB)
    {
        const FIntPoint n1(e.X + Tangent.X, e.Y + Tangent.Y);
        const FIntPoint n2(e.X - Tangent.X, e.Y - Tangent.Y);
        if (InBounds2(n1.X,n1.Y) && CachedLabels[Idx(n1.X,n1.Y,W)] == ZoneA) return false;
        if (InBounds2(n2.X,n2.Y) && CachedLabels[Idx(n2.X,n2.Y,W)] == ZoneA) return false;

        // Additional requirement: terminal empty B cell must have at least
        // one adjacent empty cell within ZoneB to ensure it opens into space.
        bool bHasEmptyNeighborInB = false;
        const FIntPoint N4[4] = { {e.X+1,e.Y},{e.X-1,e.Y},{e.X,e.Y+1},{e.X,e.Y-1} };
        for (const FIntPoint& n : N4)
        {
            if (!InBounds2(n.X, n.Y)) continue;
            if (CachedLabels[Idx(n.X,n.Y,W)] != ZoneB) continue;
            if (IsEmpty(n.X, n.Y)) { bHasEmptyNeighborInB = true; break; }
        }
        if (!bHasEmptyNeighborInB) return false;
    }

    return true;
}

bool UZonePassageGenerator::IsTooCloseToExistingPassages(const TArray<FIntPoint>& CandidateCells, int32 MinDist) const
{
    if (MinDist <= 0 || PassageMask.Num() == 0) return false;
    for (const FIntPoint& c : CandidateCells)
    {
        for (int32 dy = -MinDist; dy <= MinDist; ++dy)
        {
            const int32 rem = MinDist - FMath::Abs(dy);
            for (int32 dx = -rem; dx <= rem; ++dx)
            {
                const FIntPoint q(c.X + dx, c.Y + dy);
                if (!InBounds(q.X, q.Y)) continue;
                if (PassageMask.Contains(q)) return true;
            }
        }
    }
    return false;
}
