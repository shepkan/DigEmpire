#include "MapZoneGenerator.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Containers/Queue.h"
#include "Algo/RandomShuffle.h"
#include "DigEmpire/Map/MapGrid2D.h"

bool UMapZoneGenerator::Generate(UMapGrid2D* MapGrid,
                                 const UZoneGenSettings* Settings,
                                 UWorld* WorldForDebugDraw,
                                 TArray<int32>& OutZoneLabels)
{
	if (!ValidateInputs(MapGrid, Settings)) return false;

	const FIntPoint Size = MapGrid->GetSize();
	const int32 W = Size.X, H = Size.Y;
	const int32 NumCells = W * H;

	// Zones count
	const int32 NumZones = Settings->ZoneWeights.Num();

	// Targets from weights
	TArray<int32> Targets; Targets.SetNumZeroed(NumZones);
	ComputeTargets(NumCells, Settings->ZoneWeights, Targets);

	// Seeds
	TArray<FSeed> Seeds;
	PlaceSeedsDeterministic(Size, Settings, Seeds);
	if (Seeds.Num() != NumZones) return false;

	// Labels: -1 = unassigned; otherwise zone index [0..NumZones-1]
	OutZoneLabels.Init(-1, NumCells);

	// Initialize frontiers and area counters
	TArray<TArray<FFrontierEntry>> Frontiers;
	Frontiers.SetNum(NumZones);

	TArray<int32> Area; Area.Init(0, NumZones);

	// Claim seed cells
	for (const FSeed& S : Seeds)
	{
		const int32 id = Idx(S.X, S.Y, W);
		if (OutZoneLabels[id] == -1)
		{
			OutZoneLabels[id] = S.Zone;
			Area[S.Zone]++;
		}
		PushFreeNeighborsToFrontier(S.X, S.Y, Size, OutZoneLabels, Frontiers[S.Zone]);
	}

	// Precompute forbidden set for Zone 0 adjacency rule
	TSet<int32> ForbiddenFrom0Set(Settings->ForbiddenFromZone0);

	// Optional distance from Zone 0 for moat
	TArray<int32> DistFromZone0;
	if (Settings->MoatFromZone0 > 0)
	{
		ComputeDistFromZone0(Size, OutZoneLabels, DistFromZone0);
	}

	// Random stream for deterministic behavior
	FRandomStream RNG(Settings->RandomSeed);
	if (Settings->RandomSeed < 0)
	{
		RNG.GenerateNewSeed();
	}

	// Growth loop
	int32 Unassigned = 0;
	for (int32 v : OutZoneLabels) if (v == -1) ++Unassigned;

	const float Epsilon = 0.0001f;

	while (Unassigned > 0)
	{
		// Build pick weights per zone
		TArray<float> Weights; Weights.Init(0.f, NumZones);
		float TotalW = 0.f;

		for (int32 z = 0; z < NumZones; ++z)
		{
			auto& Frontier = Frontiers[z];
			if (Frontier.Num() == 0) { Weights[z] = 0.f; continue; }

			const bool bOverfilled = Area[z] >= int32(float(Targets[z]) * Settings->OverfillFactor);
			if (bOverfilled) { Weights[z] = 0.f; continue; }

			const int32 Need = FMath::Max(0, Targets[z] - Area[z]);
			const float w = FMath::Max(Epsilon, float(Need)) + Settings->SoftnessK;
			Weights[z] = w; TotalW += w;
		}

		if (TotalW <= 0.f)
		{
			// Fallback: greedily assign remaining cells to nearest allowed zone  (simple flood from all zones)
			// We perform a final multi-source fill without quotas but respecting constraints.
			// To keep this example compact, we relax constraints here to ensure completion.
			for (int32 i = 0; i < NumCells; ++i)
			{
				if (OutZoneLabels[i] != -1) continue;
				// Assign to the first zone that has a neighbor here (or 0)
				const int32 X = i % W, Y = i / W;
				int32 Assigned = -1;
				// Prefer neighbor zone
				if (X > 0 && OutZoneLabels[Idx(X-1,Y,W)] != -1) Assigned = OutZoneLabels[Idx(X-1,Y,W)];
				else if (X < W-1 && OutZoneLabels[Idx(X+1,Y,W)] != -1) Assigned = OutZoneLabels[Idx(X+1,Y,W)];
				else if (Y > 0 && OutZoneLabels[Idx(X,Y-1,W)] != -1) Assigned = OutZoneLabels[Idx(X,Y-1,W)];
				else if (Y < H-1 && OutZoneLabels[Idx(X,Y+1,W)] != -1) Assigned = OutZoneLabels[Idx(X,Y+1,W)];
				else Assigned = 0;
				OutZoneLabels[i] = Assigned;
			}
			break;
		}

		// Roulette-wheel sample a zone
		const float Pick = RNG.FRandRange(0.f, TotalW);
		float Acc = 0.f; int32 Z = 0;
		for (; Z < NumZones; ++Z) { Acc += Weights[Z]; if (Pick <= Acc) break; }
		if (Z >= NumZones) Z = NumZones - 1;

		// Try several candidates from Z's frontier
		auto& Frontier = Frontiers[Z];
		if (Frontier.Num() == 0) continue;

		// Random pop
		const int32 idxPop = RNG.RandRange(0, Frontier.Num() - 1);
		const FFrontierEntry C = Frontier[idxPop];
		Frontier.RemoveAtSwap(idxPop, 1, EAllowShrinking::No);

		// Already taken?
		const int32 cid = Idx(C.X, C.Y, W);
		if (OutZoneLabels[cid] != -1) continue;

		// Constraint checks
		if (ViolatesForbiddenAdjacency(Z, C.X, C.Y, Size, OutZoneLabels, ForbiddenFrom0Set))
		{
			continue;
		}
		if (WithinMoatForbidden(Z, C.X, C.Y, Size, DistFromZone0, Settings, ForbiddenFrom0Set))
		{
			continue;
		}

		// Claim
		OutZoneLabels[cid] = Z;
		Area[Z]++; Unassigned--;

		// Add neighbors to frontier
		PushFreeNeighborsToFrontier(C.X, C.Y, Size, OutZoneLabels, Frontiers[Z]);

		// Optional: slight bias for compactness — shuffle occasionally
		if ((Area[Z] & 15) == 0 && Frontier.Num() > 1)
		{
			for (int32 i = Frontier.Num() - 1; i > 0; --i)
			{
				const int32 j = RNG.RandRange(0, i);
				Frontier.Swap(i, j);
			}
		}
	}

	// Debug draw (optional)
	if (Settings->bDebugDraw && WorldForDebugDraw)
	{
		DebugDrawZones(Size, OutZoneLabels, Settings, WorldForDebugDraw);
	}

	return true;
}

bool UMapZoneGenerator::ValidateInputs(UMapGrid2D* Map, const UZoneGenSettings* Settings) const
{
	if (!Map || !Settings) return false;
	const FIntPoint Size = Map->GetSize();
	if (Size.X <= 0 || Size.Y <= 0) return false;
	if (Settings->ZoneWeights.Num() <= 0) return false;

	// Ensure ZoneIndex are 0..NumZones-1 and unique
	const int32 NumZones = Settings->ZoneWeights.Num();
	TSet<int32> Seen;
	for (const FZoneWeight& ZW : Settings->ZoneWeights)
	{
		if (ZW.ZoneIndex < 0 || ZW.ZoneIndex >= NumZones) return false;
		Seen.Add(ZW.ZoneIndex);
	}
	return Seen.Num() == NumZones;
}

void UMapZoneGenerator::ComputeTargets(int32 NumCells, const TArray<FZoneWeight>& Weights, TArray<int32>& OutTargets) const
{
	int32 SumW = 0;
	for (const auto& zw : Weights) SumW += FMath::Max(0, zw.Weight);
	if (SumW <= 0) SumW = 1;

	OutTargets.SetNumZeroed(Weights.Num());
	for (const auto& zw : Weights)
	{
		const float frac = float(FMath::Max(0, zw.Weight)) / float(SumW);
		OutTargets[zw.ZoneIndex] = FMath::RoundToInt(frac * float(NumCells));
	}
}

void UMapZoneGenerator::PlaceSeedsDeterministic(const FIntPoint& Size,
                                                const UZoneGenSettings* Settings,
                                                TArray<FSeed>& OutSeeds) const
{
	const int32 NumZones = Settings->ZoneWeights.Num();
	OutSeeds.Reset(); OutSeeds.Reserve(NumZones);

	FRandomStream RNG(Settings->RandomSeed);
	if (Settings->RandomSeed < 0)
	{
		RNG.GenerateNewSeed();
	}

	// Simple farthest-point seeding with rejection by MinSeedSeparation (Manhattan distance)
	auto IsFarEnough = [&](int32 x, int32 y)->bool
	{
		for (const FSeed& S : OutSeeds)
		{
			const int32 dx = FMath::Abs(S.X - x);
			const int32 dy = FMath::Abs(S.Y - y);
			if (dx + dy < Settings->MinSeedSeparation) return false;
		}
		return true;
	};

	// Seed for Zone 0 near a corner or center? We'll pick around center by default
	const int32 cx = Size.X / 2;
	const int32 cy = Size.Y / 2;
	OutSeeds.Add({cx, cy, /*Zone=*/0});

	// Remaining zones: sample several candidates, choose farthest from existing seeds
	for (int32 zi = 1; zi < NumZones; ++zi)
	{
		const int32 Trials = 64;
		int32 bestX = RNG.RandRange(0, Size.X - 1);
		int32 bestY = RNG.RandRange(0, Size.Y - 1);
		int32 bestScore = -1;

		for (int32 t = 0; t < Trials; ++t)
		{
			const int32 x = RNG.RandRange(0, Size.X - 1);
			const int32 y = RNG.RandRange(0, Size.Y - 1);

			if (!IsFarEnough(x,y)) continue;

			// Score = min manhattan distance to any existing seed
			int32 minD = INT32_MAX;
			for (const FSeed& S : OutSeeds)
			{
				const int32 d = FMath::Abs(S.X - x) + FMath::Abs(S.Y - y);
				minD = FMath::Min(minD, d);
			}
			if (minD > bestScore)
			{
				bestScore = minD; bestX = x; bestY = y;
			}
		}

		OutSeeds.Add({bestX, bestY, zi});
	}
}

bool UMapZoneGenerator::ViolatesForbiddenAdjacency(int32 ClaimZone, int32 X, int32 Y,
                                                   const FIntPoint& Size,
                                                   const TArray<int32>& Labels,
                                                   const TSet<int32>& ForbiddenSet) const
{
	// Only apply special rule for adjacency to Zone 0
	if (ClaimZone == 0) return false;

	// If ClaimZone is in the forbidden list relative to Zone 0, ensure the candidate is not adjacent to Zone 0
	if (!ForbiddenSet.Contains(ClaimZone)) return false;

	const int32 W = Size.X;
	// Check 4-neighbors; if any == 0 (Zone 0), forbid
	if (X > 0 && Labels[Idx(X-1,Y,W)] == 0) return true;
	if (X < Size.X-1 && Labels[Idx(X+1,Y,W)] == 0) return true;
	if (Y > 0 && Labels[Idx(X,Y-1,W)] == 0) return true;
	if (Y < Size.Y-1 && Labels[Idx(X,Y+1,W)] == 0) return true;

	return false;
}

bool UMapZoneGenerator::WithinMoatForbidden(int32 ClaimZone, int32 X, int32 Y,
                                            const FIntPoint& Size,
                                            const TArray<int32>& DistFromZone0,
                                            const UZoneGenSettings* Settings,
                                            const TSet<int32>& ForbiddenSet) const
{
	if (Settings->MoatFromZone0 <= 0) return false;
	if (ClaimZone == 0) return false;
	if (!ForbiddenSet.Contains(ClaimZone)) return false;
	if (DistFromZone0.Num() != Size.X * Size.Y) return false;

	const int32 d = DistFromZone0[Idx(X,Y,Size.X)];
	return (d >= 0 && d < Settings->MoatFromZone0);
}

void UMapZoneGenerator::PushFreeNeighborsToFrontier(int32 X, int32 Y,
                                                    const FIntPoint& Size,
                                                    const TArray<int32>& Labels,
                                                    TArray<FFrontierEntry>& Frontier) const
{
	const int32 W = Size.X;

	auto TryPush = [&](int32 nx, int32 ny)
	{
		if (nx < 0 || ny < 0 || nx >= Size.X || ny >= Size.Y) return;
		if (Labels[Idx(nx,ny,W)] == -1)
		{
			Frontier.Add({nx,ny});
		}
	};

	TryPush(X+1, Y);
	TryPush(X-1, Y);
	TryPush(X, Y+1);
	TryPush(X, Y-1);
}

void UMapZoneGenerator::ComputeDistFromZone0(const FIntPoint& Size,
                                             const TArray<int32>& Labels,
                                             TArray<int32>& OutDist) const
{
	const int32 W = Size.X, H = Size.Y;
	const int32 N = W*H;
	OutDist.Init(-1, N);

	TQueue<FIntPoint> Q;

	// Initialize queue with all Zone 0 cells (distance 0)
	for (int32 y = 0; y < H; ++y)
	{
		for (int32 x = 0; x < W; ++x)
		{
			const int32 id = Idx(x,y,W);
			if (Labels[id] == 0)
			{
				OutDist[id] = 0;
				Q.Enqueue(FIntPoint(x,y));
			}
		}
	}

	// BFS city-block distance
	while (!Q.IsEmpty())
	{
		FIntPoint c; Q.Dequeue(c);
		const int32 baseId = Idx(c.X,c.Y,W);
		const int32 baseD = OutDist[baseId];

		auto TryRelax = [&](int32 nx, int32 ny)
		{
			if (nx < 0 || ny < 0 || nx >= Size.X || ny >= Size.Y) return;
			const int32 nid = Idx(nx,ny,W);
			if (OutDist[nid] == -1)
			{
				OutDist[nid] = baseD + 1;
				Q.Enqueue(FIntPoint(nx,ny));
			}
		};

		TryRelax(c.X+1, c.Y);
		TryRelax(c.X-1, c.Y);
		TryRelax(c.X, c.Y+1);
		TryRelax(c.X, c.Y-1);
	}
}

void UMapZoneGenerator::DebugDrawZones(const FIntPoint& Size,
                                       const TArray<int32>& Labels,
                                       const UZoneGenSettings* Settings,
                                       UWorld* World) const
{
	if (!World || !Settings) return;

	const int32 W = Size.X, H = Size.Y;
	const int32 N = W * H;
	const int32 NumZones = Settings->ZoneWeights.Num();
	if (W <= 0 || H <= 0 || Labels.Num() != N || NumZones <= 0) return;

	// Build a simple HSV palette (one color per zone).
	TArray<FColor> Palette; 
	Palette.SetNum(NumZones);
	for (int32 z = 0; z < NumZones; ++z)
	{
		const uint8 H8 = uint8((255 * z) / FMath::Max(1, NumZones));
		const FLinearColor Lin = FLinearColor::MakeFromHSV8(H8, 200, 255);
		Palette[z] = Lin.ToFColor(true);
	}

	const float R = Settings->DebugSphereRadiusUU;
	const float Life = Settings->DebugLifetime;
	const float Tile = Settings->TileSizeUU;
	const float Z = Settings->DebugZOffset;

	auto IsBoundary = [&](int32 x, int32 y, int32 zone) -> bool
	{
		// Check 4-neighborhood for a different zone id
		// Left
		if (x > 0)          { const int32 z2 = Labels[(x - 1) + y * W];     if (z2 != -1 && z2 != zone) return true; }
		// Right
		if (x < W - 1)      { const int32 z2 = Labels[(x + 1) + y * W];     if (z2 != -1 && z2 != zone) return true; }
		// Down
		if (y > 0)          { const int32 z2 = Labels[x + (y - 1) * W];     if (z2 != -1 && z2 != zone) return true; }
		// Up
		if (y < H - 1)      { const int32 z2 = Labels[x + (y + 1) * W];     if (z2 != -1 && z2 != zone) return true; }

		return false;
	};

	for (int32 y = 0; y < H; ++y)
	{
		for (int32 x = 0; x < W; ++x)
		{
			const int32 id = x + y * W;
			const int32 z = Labels[id];
			if (z < 0 || z >= NumZones) continue;

			if (!IsBoundary(x, y, z)) continue;

			const FVector P(x * Tile, y * Tile, Z);
			DrawDebugSphere(
				World,
				P,
				R,
				12,
				Palette[z],
				/*bPersistentLines=*/Life <= 0.f,
				/*LifeTime=*/Life,
				/*DepthPriority=*/0,
				/*Thickness=*/1.0f
			);
		}
	}
}
