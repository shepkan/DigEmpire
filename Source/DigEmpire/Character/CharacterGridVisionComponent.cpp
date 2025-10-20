#include "CharacterGridVisionComponent.h"

#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "TimerManager.h"

#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"

UCharacterGridVisionComponent::UCharacterGridVisionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoActivate = true;
}

void UCharacterGridVisionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!MapComponent) { TryAutoFindMap(); }

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
            Payload.Cells.Add(Entry);

            if (!bWasViewed)
            {
                NewlySeen.Add(Entry);
            }
        }
    }

    UGameplayMessageSubsystem::Get(this).BroadcastMessage(VisionChannel, Payload);

    if (FirstSeenChannel.IsValid() && NewlySeen.Num() > 0)
    {
        FCellsFirstSeenMessage First;
        First.SourceActor = GetOwner();
        First.Cells = MoveTemp(NewlySeen);
        UGameplayMessageSubsystem::Get(this).BroadcastMessage(FirstSeenChannel, First);
    }
}
