#include "ZoneConnectivityFixer.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DrawDebugHelpers.h"

bool UZoneConnectivityFixer::Generate(UMapGrid2D* MapGrid,
                                      const TArray<int32>& ZoneLabels,
                                      const TArray<FGameplayTag>& ImmutableObjectTags,
                                      bool bDebugDraw,
                                      float DebugTileSizeUU,
                                      float DebugZOffset,
                                      float DebugSphereRadiusUU,
                                      float DebugLifetime)
{
    if (!MapGrid) return false;
    const FIntPoint Size = MapGrid->GetSize();
    const int32 W = Size.X, H = Size.Y;
    if (W <= 0 || H <= 0) return false;
    if (ZoneLabels.Num() != W * H) return false;

    // Determine max zone id
    int32 MaxZoneId = 0; for (int v : ZoneLabels) if (v > MaxZoneId) MaxZoneId = v;

    TArray<uint8> Open, ImmWall, MutWall;
    Open.SetNumZeroed(W * H);
    ImmWall.SetNumZeroed(W * H);
    MutWall.SetNumZeroed(W * H);

    // Temporary arrays for component labels
    TArray<int32> Comp; Comp.Init(-1, W*H);
    TArray<FNode> Q; Q.Reserve(W*H);

    UWorld* World = MapGrid->GetWorld();
    for (int32 ZoneId = 0; ZoneId <= MaxZoneId; ++ZoneId)
    {
        // Build masks for this zone
        Open.Init(0, W*H); ImmWall.Init(0, W*H); MutWall.Init(0, W*H); Comp.Init(-1, W*H);
        BuildMasksForZone(MapGrid, ZoneLabels, ZoneId, ImmutableObjectTags, Open, ImmWall, MutWall);

        // Label connected components among open cells (4-neighbors)
        int32 compCount = 0;
        TArray<int32> compSizes;
        for (int32 y = 0; y < H; ++y)
        for (int32 x = 0; x < W; ++x)
        {
            const int id = Idx(x,y,W);
            if (ZoneLabels[id] != ZoneId) continue;
            if (!Open[id]) continue;
            if (Comp[id] != -1) continue;

            const int curComp = compCount++;
            int32 size = 0;
            Q.Reset(); Q.Add({(int16)x,(int16)y}); Comp[id] = curComp;
            while (!Q.IsEmpty())
            {
                const FNode n = Q.Pop(EAllowShrinking::No);
                ++size;
                const int nid = Idx(n.X, n.Y, W);
                // 4-neighbors
                auto Push = [&](int nx, int ny){
                    if (nx<0||ny<0||nx>=W||ny>=H) return;
                    const int iid = Idx(nx,ny,W);
                    if (ZoneLabels[iid]!=ZoneId) return;
                    if (!Open[iid]) return;
                    if (Comp[iid]!=-1) return;
                    Comp[iid]=curComp; Q.Add({(int16)nx,(int16)ny});
                };
                Push(n.X+1,n.Y); Push(n.X-1,n.Y); Push(n.X,n.Y+1); Push(n.X,n.Y-1);
            }
            compSizes.Add(size);
        }

        if (compCount <= 1) continue; // already connected or empty

        // Pick largest component as main
        int32 mainComp = 0; int32 bestSize = -1;
        for (int32 i=0;i<compSizes.Num();++i){ if (compSizes[i]>bestSize){bestSize=compSizes[i]; mainComp=i;} }
        TArray<uint8> compConnected; compConnected.Init(0, compCount); compConnected[mainComp]=1;

        // Multi-source 0-1 BFS to connect remaining components iteratively
        TArray<int32> Dist; Dist.Init(INT32_MAX, W*H);
        TArray<int32> Prev; Prev.Init(-1, W*H);
        TArray<uint8> InQueue; InQueue.Init(0, W*H);

        auto ConnectOne = [&]() -> bool
        {
            // Initialize deque with all cells of connected components
            TArray<int32> Deque; Deque.Reserve(W*H);
            auto PushFront = [&](int id){ Deque.Insert(id,0); };
            auto PushBack  = [&](int id){ Deque.Add(id); };
            auto PopFront  = [&](){ int id=Deque[0]; Deque.RemoveAt(0); return id; };

            for (int32 i=0;i<W*H;++i){ Dist[i]=INT32_MAX; Prev[i]=-1; InQueue[i]=0; }

            for (int32 i=0;i<W*H;++i)
            {
                if (ZoneLabels[i]!=ZoneId) continue;
                if (!Open[i]) continue;
                if (Comp[i]>=0 && compConnected[Comp[i]])
                {
                    Dist[i]=0; InQueue[i]=1; PushFront(i);
                }
            }
            if (Deque.Num()==0) return false;

            int32 meetId = -1; int32 meetComp = -1;
            while (Deque.Num()>0)
            {
                const int id = PopFront(); InQueue[id]=0;
                const int x = id % W; const int y = id / W;

                // If we reached an unconnected component, stop
                if (Comp[id]>=0 && !compConnected[Comp[id]]) { meetId=id; meetComp=Comp[id]; break; }

                auto Relax = [&](int nx, int ny){
                    if (nx<0||ny<0||nx>=W||ny>=H) return;
                    const int nid = Idx(nx,ny,W);
                    if (ZoneLabels[nid]!=ZoneId) return;
                    if (ImmWall[nid]) return; // cannot cross immutable walls
                    const int w = MutWall[nid] ? 1 : (Open[nid] ? 0 : 1); // non-open but mutable treated as wall cost 1
                    const int nd = Dist[id] + w;
                    if (nd < Dist[nid])
                    {
                        Dist[nid]=nd; Prev[nid]=id;
                        if (w==0) { Deque.Insert(nid,0); }
                        else { Deque.Add(nid); }
                    }
                };
                Relax(x+1,y); Relax(x-1,y); Relax(x,y+1); Relax(x,y-1);
            }

            if (meetId<0 || meetComp<0) return false;

            // Reconstruct and carve along path where MutWall==1 or (not open and not immutable)
            int cur = meetId;
            while (cur>=0 && Prev[cur]>=0)
            {
                if (MutWall[cur])
                {
                    const int cx = cur % W; const int cy = cur / W;
                    MapGrid->RemoveObjectAt(cx, cy);
                    Open[cur]=1; MutWall[cur]=0;
                }
                cur = Prev[cur];
            }

            // Mark this component connected
            compConnected[meetComp]=1;
            return true;
        };

        // Connect until all components connected or no path
        int remaining = compCount - 1;
        while (remaining>0)
        {
            if (!ConnectOne()) break;
            remaining = 0; for (int i=0;i<compCount;++i) if (!compConnected[i]) ++remaining;
        }

        // Debug draw all open cells that remain in unconnected components
        if (bDebugDraw && World)
        {
            for (int32 y = 0; y < H; ++y)
            for (int32 x = 0; x < W; ++x)
            {
                const int id = Idx(x, y, W);
                // Skip cells that are not in the current zone and any cells without a zone label
                if (ZoneLabels[id] < 0 || ZoneLabels[id] != ZoneId) continue;
                if (!Open[id]) continue;
                const int c = Comp[id];
                // Draw if component index is invalid or not connected
                if (c < 0 || !compConnected.IsValidIndex(c) || !compConnected[c])
                {
                    const FVector P(x * DebugTileSizeUU, y * DebugTileSizeUU, DebugZOffset);
                    DrawDebugSphere(World, P, DebugSphereRadiusUU, 12, FColor::Red, DebugLifetime <= 0.f, DebugLifetime);
                }
            }
        }
    }

    return true;
}

bool UZoneConnectivityFixer::IsZoneConnected(const UMapGrid2D* MapGrid,
                                             int32 ZoneId) const
{
    if (!MapGrid) return false;
    const FIntPoint Size = MapGrid->GetSize();
    const int32 W = Size.X, H = Size.Y;
    const int32 N = W*H;

    const TArray<FZonePassage>& Passages = MapGrid->GetPassages();

    auto IsOpen = [&](int x, int y)->bool
    {
        if (x<0||y<0||x>=W||y>=H) return false;
        const int id = Idx(x,y,W);
        if (MapGrid->GetZoneAt(x, y) != ZoneId) return false;
        FGameplayTag T; int32 D=0; 
        if (!MapGrid->GetObjectAt(x,y,T,D)) return true; // no wall
        // If it's a passage cell, treat as open even if object existed (shouldn't normally)
        for (const FZonePassage& P : Passages)
        {
            for (const FIntPoint& C : P.Cells)
            {
                if (C.X==x && C.Y==y) return true;
            }
        }
        return false;
    };

    // Find first open cell
    int start=-1;
    for (int y=0;y<H && start==-1;++y)
    for (int x=0;x<W && start==-1;++x)
    {
        if (IsOpen(x,y)) start = Idx(x,y,W);
    }
    if (start==-1) return true; // no open cells -> trivially connected

    TArray<uint8> Vis; Vis.Init(0,N);
    TQueue<int32> Q; Q.Enqueue(start); Vis[start]=1;
    auto Push=[&](int nx,int ny){ if (IsOpen(nx,ny)){ const int id=Idx(nx,ny,W); if(!Vis[id]){ Vis[id]=1; Q.Enqueue(id);} } };
    while(!Q.IsEmpty())
    {
        int32 u; Q.Dequeue(u);
        const int ux=u%W, uy=u/W;
        Push(ux+1,uy); Push(ux-1,uy); Push(ux,uy+1); Push(ux,uy-1);
    }

    // Verify all open cells visited
    for (int y=0;y<H;++y)
    for (int x=0;x<W;++x)
    {
        if (IsOpen(x,y))
        {
            if (!Vis[Idx(x,y,W)]) return false;
        }
    }
    return true;
}

void UZoneConnectivityFixer::BuildMasksForZone(UMapGrid2D* Map,
                                               const TArray<int32>& Labels,
                                               int32 ZoneId,
                                               const TArray<FGameplayTag>& ImmutableObjectTags,
                                               TArray<uint8>& Open,
                                               TArray<uint8>& ImmWall,
                                               TArray<uint8>& MutWall)
{
    const FIntPoint Size = Map->GetSize();
    const int32 W = Size.X, H = Size.Y;
    const TArray<FRoomInfo>& Rooms = Map->GetRooms();
    const TArray<FZonePassage>& Passages = Map->GetPassages();

    TSet<FIntPoint> RoomInterior; // treat as non-traversable for connectivity (immutable block)
    TSet<FIntPoint> RoomWalls;

    // Rooms: interiors NON-TRAVERSABLE; walls immutable wall; entrance is also non-traversable here
    for (const FRoomInfo& R : Rooms)
    {
        if (R.ZoneId != ZoneId) continue;
        for (int32 dy = 0; dy < R.Size.Y; ++dy)
        for (int32 dx = 0; dx < R.Size.X; ++dx)
        {
            RoomInterior.Add(FIntPoint(R.TopLeft.X + dx, R.TopLeft.Y + dy));
        }
        const int x0=R.TopLeft.X, y0=R.TopLeft.Y, w=R.Size.X, h=R.Size.Y;
        // Edges
        for (int dx=0; dx<w; ++dx)
        {
            RoomWalls.Add(FIntPoint(x0+dx, y0));
            RoomWalls.Add(FIntPoint(x0+dx, y0+h-1));
        }
        for (int dy=1; dy<h-1; ++dy)
        {
            RoomWalls.Add(FIntPoint(x0, y0+dy));
            RoomWalls.Add(FIntPoint(x0+w-1, y0+dy));
        }
        // Entrance: mark as non-traversable in connectivity (do not go through door)
        RoomWalls.Add(R.Entrance);
        RoomInterior.Add(R.Entrance);

        // Force-free the cell directly outside the entrance within the same zone.
        // This guarantees the room has an exit into traversable space.
        FIntPoint Outside = R.Entrance;
        if (R.Entrance.Y == y0)            { Outside.Y -= 1; }           // entrance on top edge -> outside north
        else if (R.Entrance.Y == y0 + h - 1) { Outside.Y += 1; }         // bottom edge -> outside south
        else if (R.Entrance.X == x0)       { Outside.X -= 1; }           // left edge -> outside west
        else if (R.Entrance.X == x0 + w - 1) { Outside.X += 1; }         // right edge -> outside east

        if (Map->IsInBounds(Outside.X, Outside.Y))
        {
            const int oid = Idx(Outside.X, Outside.Y, W);
            if (Labels.IsValidIndex(oid) && Labels[oid] == ZoneId)
            {
                FGameplayTag ObjTag; int32 ObjDur = 0;
                if (Map->GetObjectAt(Outside.X, Outside.Y, ObjTag, ObjDur))
                {
                    Map->RemoveObjectAt(Outside.X, Outside.Y);
                }
            }
        }
    }

    // Passages: OPEN (targets to reach); do not modify openness here
    for (const FZonePassage& P : Passages)
    {
        for (const FIntPoint& C : P.Cells) { /* keep open */ }
    }

    for (int32 y=0; y<H; ++y)
    for (int32 x=0; x<W; ++x)
    {
        const int id = Idx(x,y,W);
        if (Labels[id] != ZoneId) { ImmWall[id]=1; continue; } // treat outside as wall

        // Room interior/door: non-traversable, immutable
        if (RoomInterior.Contains(FIntPoint(x,y))) { ImmWall[id]=1; continue; }

        // Object check
        FGameplayTag Obj; int32 Dur=0; const bool bHasObj = Map->GetObjectAt(x,y,Obj,Dur);
        if (bHasObj && ImmutableObjectTags.Contains(Obj))
        {
            // Check zone-boundary adjacency (wall between zones) -> immutable
            bool bBetweenZones = false;
            if (x>0 && Labels[Idx(x-1,y,W)]!=ZoneId) bBetweenZones=true;
            if (x<W-1 && Labels[Idx(x+1,y,W)]!=ZoneId) bBetweenZones=true;
            if (y>0 && Labels[Idx(x,y-1,W)]!=ZoneId) bBetweenZones=true;
            if (y<H-1 && Labels[Idx(x,y+1,W)]!=ZoneId) bBetweenZones=true;
            if (bBetweenZones || RoomWalls.Contains(FIntPoint(x,y)))
            {
                ImmWall[id]=1; // immutable wall
            }
            else
            {
                MutWall[id]=1; // mutable wall inside the zone
            }
        }
        else
        {
            Open[id]=1; // empty cell
        }
    }
}
