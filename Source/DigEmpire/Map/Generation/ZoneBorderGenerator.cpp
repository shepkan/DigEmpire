#include "ZoneBorderGenerator.h"
#include "DrawDebugHelpers.h"
#include "Algo/RandomShuffle.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UZoneBorderGenerator::Generate(UMapGrid2D* MapGrid,
                                    const TArray<int32>& ZoneLabels,
                                    const UZoneBorderSettings* Settings)
{
	if (!ValidateInputs(MapGrid, ZoneLabels, Settings))
		return false;

	CachedLabels = ZoneLabels;
	CachedSize = MapGrid->GetSize();
	Passages.Reset();

	// ZoneBorderGenerator.cpp (inside Generate)
	TMap<FIntPoint, TSet<FIntPoint>> PairToA, PairToB;
	CollectZoneBoundaries(CachedLabels, PairToA, PairToB);

	// 1) Place walls first
	PlaceWallsWithThickness(MapGrid, Settings, PairToA);

	// 2) Then carve passages
	ChooseAndCarvePassages(MapGrid, Settings, PairToA, PairToB);

	return true;
}

TArray<FIntPoint> UZoneBorderGenerator::GetFreeCellsForZone(UMapGrid2D* MapGrid, int32 ZoneId) const
{
	TArray<FIntPoint> Out;
	if (!MapGrid || CachedLabels.Num() != CachedSize.X * CachedSize.Y) return Out;
	if (ZoneId < 0) return Out;

	const int32 W = CachedSize.X, H = CachedSize.Y;

	for (int32 y = 0; y < H; ++y)
	{
		for (int32 x = 0; x < W; ++x)
		{
			const int32 id = Idx(x,y,W);
			if (CachedLabels[id] != ZoneId) continue;

			FGameplayTag Obj; int32 Durability = 0;
			const bool bHasObj = MapGrid->GetObjectAt(x, y, Obj, Durability);
			if (bHasObj && Obj.IsValid() && Durability > 0) continue;

			Out.Add(FIntPoint(x,y));
		}
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

	auto PairKey = [](int32 A, int32 B)->FIntPoint
	{
		return (A < B) ? FIntPoint(A,B) : FIntPoint(B,A);
	};

	for (int32 y = 0; y < H; ++y)
	{
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
					// Put the left cell into the lower-id side set
					if (Key.X == z)  { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x,   y)); }
					else             { OutPairToA.FindOrAdd(Key).Add(FIntPoint(x+1, y)); }
					// And mirror to B side
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
}

void UZoneBorderGenerator::ChooseAndCarvePassages(
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

			// Update protection mask: passage cells + dilation to protect from any walls & enforce spacing
			for (const FIntPoint& c : Passages.Last().Cells) PassageMask.Add(c);
			DilateMask(PassageMask, FMath::Max(0, Settings->BorderThickness - 1)); // shield from thick borders
			DilateMask(PassageMask, Settings->MinPassageDistance);                  // spacing for future passages

			// Debug
			if (Settings->bDebugDrawPassages)
			{
				DebugDrawPassageCells(Settings, Passages.Last().Cells, FColor::MakeRandomColor(), Map->GetWorld());
			}

			bCarved = true;
		}
	}
}

void UZoneBorderGenerator::PlaceWallsWithThickness(UMapGrid2D* Map,
												   const UZoneBorderSettings* Settings,
												   const TMap<FIntPoint, TSet<FIntPoint>>& PairToA)
{
	// PassageMask already contains the carved cells plus protective dilation.
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
			if (PassageMask.Contains(p)) continue; // keep passage & buffer free
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
				if (PassageMask.Contains(n)) continue;     // do not overlap an opening

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

void UZoneBorderGenerator::ClearCell(UMapGrid2D* Map, int32 X, int32 Y) const
{
	Map->RemoveObjectAt(X, Y);
}

void UZoneBorderGenerator::DebugDrawPassageCells(const UZoneBorderSettings* Settings, const TArray<FIntPoint>& Cells, const FColor& Color, UWorld* World) const
{
	if (!Settings->bDebugDrawPassages || !World) return;

	for (const FIntPoint& c : Cells)
	{
		const FVector P(c.X * Settings->DebugTileSizeUU, c.Y * Settings->DebugTileSizeUU, Settings->DebugZOffset);
		DrawDebugSphere(World, P, Settings->DebugSphereRadiusUU, 12, Color, Settings->DebugLifetime <= 0.f, Settings->DebugLifetime);
	}
}
void UZoneBorderGenerator::DilateMask(TSet<FIntPoint>& InOutMask, int32 Radius) const
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

bool UZoneBorderGenerator::IsTooCloseToExistingPassages(const TArray<FIntPoint>& CandidateCells, int32 MinDist) const
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

// Any blocking object = occupied
bool UZoneBorderGenerator::IsOccupied(UMapGrid2D* Map, int32 X, int32 Y) const
{
	FGameplayTag Obj; int32 Dur; 
	if (!Map->GetObjectAt(X, Y, Obj, Dur)) return false;
	return Obj.IsValid() && Dur > 0;
}

/**
 * Build a preview of the cells to clear for a perpendicular passage of given width.
 * We already have walls placed. From the anchor, for each column of the stripe:
 *  - step inward into ZoneA along its inward normal until we hit the first EMPTY cell **in ZoneA**,
 *    accumulating all occupied cells on the way (they will be cleared);
 *  - step outward into ZoneB (across boundary) until first EMPTY cell **in ZoneB**, accumulating occupied cells.
 * If either side hits an EMPTY cell belonging to a wrong zone (or leaves bounds), we FAIL this anchor.
 * We only PREVIEW here—no modifications to the map yet.
 */
bool UZoneBorderGenerator::BuildCarveStripePreview(
	UMapGrid2D* Map,
	const FIntPoint& AnchorA,
	bool bVerticalBoundary,      // true => boundary normal is X; passage tangent is Y
	int dxToB, int dyToB,        // unit vector from A to B across the boundary (±1,0) or (0,±1)
	int32 PassageWidth,
	int32 ZoneA, int32 ZoneB,
	TArray<FIntPoint>& OutCellsA,
	TArray<FIntPoint>& OutCellsB) const
{
	OutCellsA.Reset();
	OutCellsB.Reset();

	const int32 W = CachedSize.X, H = CachedSize.Y;
	auto InBounds2 = [&](int x, int y){ return x>=0 && y>=0 && x<W && y<H; };

	// Helper: check if a cell has no blocking object
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

	// We collect terminal empty cells for both sides to validate lateral leakage.
	TArray<FIntPoint> TerminalEmptyA;
	TArray<FIntPoint> TerminalEmptyB;

	for (int t = -Half; t <= Half; ++t)
	{
		const FIntPoint BaseA(AnchorA.X + Tangent.X * t, AnchorA.Y + Tangent.Y * t);
		if (!InBounds2(BaseA.X, BaseA.Y)) return false;

		// --- A side: walk inside Zone A from BaseA (step=0) until first EMPTY A cell.
		{
			bool bStopped = false;
			int step = 0;
			while (true)
			{
				const FIntPoint p(BaseA.X + InwardA.X * step, BaseA.Y + InwardA.Y * step);
				if (!InBounds2(p.X, p.Y)) return false;

				const int lab = CachedLabels[Idx(p.X, p.Y, W)];
				if (lab != ZoneA) return false; // left Zone A before hitting an empty A cell

				if (IsOccupied(Map, p.X, p.Y))
				{
					OutCellsA.Add(p); // clear this blocking cell
					++step;
					continue;
				}
				// First EMPTY A-cell reached → remember for leakage check and stop this column on A side
				TerminalEmptyA.Add(p);
				bStopped = true;
				break;
			}
			if (!bStopped) return false;
		}

		// --- B side: start at step=1 (first Zone B cell across boundary) until first EMPTY B cell.
		{
			bool bStopped = false;
			int step = 1; // IMPORTANT: start across the boundary
			while (true)
			{
				const FIntPoint p(BaseA.X + OutwardB.X * step, BaseA.Y + OutwardB.Y * step);
				if (!InBounds2(p.X, p.Y)) return false;

				const int lab = CachedLabels[Idx(p.X, p.Y, W)];
				if (lab != ZoneB) return false; // entered a wrong zone (e.g., third zone near corner)

				if (IsOccupied(Map, p.X, p.Y))
				{
					OutCellsB.Add(p); // clear until we reach empty B
					++step;
					continue;
				}
				// First EMPTY B-cell reached → remember for leakage check and stop this column on B side
				TerminalEmptyB.Add(p);
				bStopped = true;
				break;
			}
			if (!bStopped) return false;
		}
	}

	// --- Leakage check around the mouths (terminal empties) on BOTH sides.
	// If any adjacent EMPTY cell belongs to a third zone (not A/B), reject this anchor.
	auto FailsLeakCheck = [&](const FIntPoint& Mouth)->bool
	{
		static const FIntPoint N4[4] = { {1,0},{-1,0},{0,1},{0,-1} };
		for (const FIntPoint& o : N4)
		{
			const FIntPoint q(Mouth.X + o.X, Mouth.Y + o.Y);
			if (!InBounds2(q.X, q.Y)) continue;

			if (!IsEmpty(q.X, q.Y)) continue; // blocked: fine

			const int lab = CachedLabels[Idx(q.X, q.Y, W)];
			if (lab != ZoneA && lab != ZoneB)
			{
				// Adjacent empty opens directly into a third zone → bad passage
				return true;
			}
		}
		return false;
	};

	for (const FIntPoint& e : TerminalEmptyA)
		if (FailsLeakCheck(e)) return false;

	for (const FIntPoint& e : TerminalEmptyB)
		if (FailsLeakCheck(e)) return false;

	return true;
}



