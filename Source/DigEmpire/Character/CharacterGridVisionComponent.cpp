#include "CharacterGridVisionComponent.h"

#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TimerManager.h"

#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/BusEvents/LuminanceMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"
#include "DigEmpire/Character/DwarfLightComponent.h"
#include "DigEmpire/Character/DwarfLightSettings.h"

UCharacterGridVisionComponent::UCharacterGridVisionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoActivate = true;
    VisionChannel = TAG_Character_Vision;
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;
    LuminanceChannel = TAG_Render_LuminanceUpdate;
}

void UCharacterGridVisionComponent::Cheat_LockMaxVisibility(int32 LockedRadius)
{
    if (!bVisionLockedByCheat)
    {
        VisionRadiusBeforeCheat = VisionRadiusCells;
    }
    bVisionLockedByCheat = true;
    VisionRadiusCells = FMath::Max(0, LockedRadius);
}

void UCharacterGridVisionComponent::Cheat_UnlockVisibility()
{
    bVisionLockedByCheat = false;
    if (VisionRadiusBeforeCheat >= 0)
    {
        VisionRadiusCells = VisionRadiusBeforeCheat;
    }
    VisionRadiusBeforeCheat = -1;
}

void UCharacterGridVisionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!MapComponent) { TryAutoFindMap(); }

    // Ensure native defaults even if Blueprint overrides left them empty
    VisionChannel = TAG_Character_Vision;
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;
    LuminanceChannel = TAG_Render_LuminanceUpdate;

    if (VisionIntervalSeconds > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            VisionTimerHandle,
            this,
            &UCharacterGridVisionComponent::DoVisionTick,
            VisionIntervalSeconds,
            /*bLoop=*/true
        );
    }
}

void UCharacterGridVisionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (VisionTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(VisionTimerHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void UCharacterGridVisionComponent::TryAutoFindMap()
{
    if (MapComponent) return;
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            if (UMapGrid2DComponent* Comp = It->FindComponentByClass<UMapGrid2DComponent>())
            {
                MapComponent = Comp;
                return;
            }
        }
    }
}

FVector2D UCharacterGridVisionComponent::WorldToGridFloat(const FVector& WorldLocation) const
{
    return FVector2D(WorldLocation.X / TileSize, WorldLocation.Y / TileSize);
}

void UCharacterGridVisionComponent::DoVisionTick()
{
    if (!GetOwner()) return;
    if (!MapComponent || !MapComponent->IsMapReady()) return;
    if (!VisionChannel.IsValid()) return; // nothing to publish to

    const FVector OwnerWorld = GetOwner()->GetActorLocation();
    const FVector2D GridF = WorldToGridFloat(OwnerWorld);

    const int32 CenterX = FMath::RoundToInt(GridF.X);
    const int32 CenterY = FMath::RoundToInt(GridF.Y);
    const int32 R = FMath::Max(0, VisionRadiusCells);

    FCharacterGridVisionMessage Payload;
    Payload.SourceActor = GetOwner();
    Payload.Center = FIntPoint(CenterX, CenterY);
    Payload.RadiusCells = R;
    Payload.RadiusLayers.SetNum(R + 1);

    // Iterate square bounds and filter by euclidean distance
    const int32 MinX = CenterX - R;
    const int32 MaxX = CenterX + R;
    const int32 MinY = CenterY - R;
    const int32 MaxY = CenterY + R;

    const int32 R2 = R * R;

    TArray<FGridCellWithCoord> NewlySeen;

    for (int32 y = MinY; y <= MaxY; ++y)
    {
        for (int32 x = MinX; x <= MaxX; ++x)
        {
            const int32 dx = x - CenterX;
            const int32 dy = y - CenterY;
            if (dx*dx + dy*dy > R2) continue; // outside circle
            if (!MapComponent->IsInBounds(x, y)) continue;

            bool bWasViewed = false;
            if (UMapGrid2D* Map = MapComponent->GetMap())
            {
                FMapCell Tmp;
                if (Map->GetCell(x, y, Tmp))
                {
                    bWasViewed = Tmp.bVieved;
                }
                Map->SetViewedAt(x, y, true);
            }

            FGridCellWithCoord Entry;
            Entry.Coord = FIntPoint(x, y);
            MapComponent->GetCell(x, y, Entry.Cell); // if fails, leaves default cell

            const int32 d2 = dx*dx + dy*dy;
            int32 RingIndex = 0;
            if (d2 > 0)
            {
                RingIndex = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(d2)));
                RingIndex = FMath::Clamp(RingIndex, 0, R);
            }
            Payload.RadiusLayers[RingIndex].Cells.Add(Entry);

            if (!bWasViewed)
            {
                NewlySeen.Add(Entry);
            }
        }
    }

    // Publish vision before luminance so renderer can build instances first
    UGameplayMessageSubsystem::Get(this).BroadcastMessage(VisionChannel, Payload);

    if (FirstSeenChannel.IsValid() && NewlySeen.Num() > 0)
    {
        FCellsFirstSeenMessage First;
        First.SourceActor = GetOwner();
        First.Cells = MoveTemp(NewlySeen);
        UGameplayMessageSubsystem::Get(this).BroadcastMessage(FirstSeenChannel, First);
    }

    // Build visible set and diff with last
    TSet<FIntPoint> VisibleNow;
    for (const FGridCellsRing& Ring : Payload.RadiusLayers)
    {
        for (const FGridCellWithCoord& Entry : Ring.Cells)
        {
            VisibleNow.Add(Entry.Coord);
        }
    }

    TArray<FIntPoint> LeftVisibility;
    for (const FIntPoint& Prev : LastVisibleCells)
    {
        if (!VisibleNow.Contains(Prev))
        {
            LeftVisibility.Add(Prev);
        }
    }

    // Resolve luminance per ring from DwarfLight settings
    TArray<float> LuminanceByRing;
    LuminanceByRing.SetNum(Payload.RadiusLayers.Num());
    for (int32 i = 0; i < LuminanceByRing.Num(); ++i) { LuminanceByRing[i] = 1.0f; }

    UDwarfLightComponent* Light = CachedLight.Get();
    if (!Light && GetOwner())
    {
        Light = GetOwner()->FindComponentByClass<UDwarfLightComponent>();
        if (Light) { CachedLight = Light; }
    }
    if (Light && Light->Settings)
    {
        const FDwarfLightVisibilityConfig* Cfg = Light->Settings->FindBestConfig(Light->LightPower);
        if (Cfg)
        {
            // Copy luminance values, pad/truncate to match rings
            for (int32 i = 0; i < LuminanceByRing.Num(); ++i)
            {
                float Val = 1.0f;
                if (Cfg->LuminanceByRing.IsValidIndex(i))
                {
                    Val = Cfg->LuminanceByRing[i];
                }
                else if (Cfg->LuminanceByRing.Num() > 0)
                {
                    Val = Cfg->LuminanceByRing.Last();
                }
                LuminanceByRing[i] = Val;
            }
        }
    }

    // Publish luminance update
    if (LuminanceChannel.IsValid())
    {
        FLuminanceUpdateMessage Lum;
        Lum.SourceActor = GetOwner();
        Lum.RadiusLayers = Payload.RadiusLayers;
        Lum.LuminanceByRing = MoveTemp(LuminanceByRing);
        Lum.CellsLeftVisibility = MoveTemp(LeftVisibility);
        UGameplayMessageSubsystem::Get(this).BroadcastMessage(LuminanceChannel, Lum);
    }

    LastVisibleCells = MoveTemp(VisibleNow);
}

void UCharacterGridVisionComponent::ForceVisionUpdate()
{
    DoVisionTick();
}
