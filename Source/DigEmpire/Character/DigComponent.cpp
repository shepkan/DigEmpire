#include "DigComponent.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

#include "DigEmpire/Config/DEConstants.h"
#include "DigEmpire/Character/GridMovementComponent.h"
#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"

UDigComponent::UDigComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bAutoActivate = true;
    TileSize = DEConstants::TileSizeUU;
}

void UDigComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!MapComponent) { TryAutoFindMap(); }
    if (!MovementComponent) { TryAutoFindMovement(); }
}

void UDigComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopDig();
    Super::EndPlay(EndPlayReason);
}

void UDigComponent::TryAutoFindMap()
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

void UDigComponent::TryAutoFindMovement()
{
    if (MovementComponent) return;
    if (AActor* Owner = GetOwner())
    {
        MovementComponent = Owner->FindComponentByClass<UGridMovementComponent>();
    }
}

FVector2D UDigComponent::WorldToGridFloat(const FVector& WorldLocation) const
{
    return FVector2D(WorldLocation.X / TileSize, WorldLocation.Y / TileSize);
}

void UDigComponent::StartDigInternal(EDigDirection Dir)
{
    if (bIsDigging && CurrentDirection == Dir)
    {
        return;
    }

    // Restart in a new direction
    StopDig();

    CurrentDirection = Dir;
    bIsDigging = true;

    if (MovementComponent)
    {
        MovementComponent->SetExternalMovementBlocked(true);
    }

    if (DigIntervalSeconds > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            DigTimerHandle,
            this,
            &UDigComponent::DoDigTick,
            DigIntervalSeconds,
            /*bLoop=*/true
        );
    }
}

void UDigComponent::StartDigUp()   { StartDigInternal(EDigDirection::Up); }
void UDigComponent::StartDigDown() { StartDigInternal(EDigDirection::Down); }
void UDigComponent::StartDigLeft() { StartDigInternal(EDigDirection::Left); }
void UDigComponent::StartDigRight(){ StartDigInternal(EDigDirection::Right); }

void UDigComponent::StopDig()
{
    if (!bIsDigging)
    {
        return;
    }

    bIsDigging = false;
    if (DigTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(DigTimerHandle);
    }
    if (MovementComponent)
    {
        MovementComponent->SetExternalMovementBlocked(false);
    }
}

void UDigComponent::DoDigTick()
{
    if (!GetOwner()) return;
    if (!MapComponent || !MapComponent->IsMapReady()) return;

    // Determine current center cell and target cell one step in CurrentDirection
    const FVector OwnerWorld = GetOwner()->GetActorLocation();
    const FVector2D GridF = WorldToGridFloat(OwnerWorld);
    int32 GX = FMath::RoundToInt(GridF.X);
    int32 GY = FMath::RoundToInt(GridF.Y);

    switch (CurrentDirection)
    {
        case EDigDirection::Up:    ++GY; break;
        case EDigDirection::Down:  --GY; break;
        case EDigDirection::Left:  --GX; break;
        case EDigDirection::Right: ++GX; break;
    }

    if (!MapComponent->IsInBounds(GX, GY))
    {
        return; // nothing to dig out of bounds
    }

    // Damage object if present
    bool bDestroyed = false;
    MapComponent->DamageObjectAt(GX, GY, DamagePerTick, bDestroyed);
}

