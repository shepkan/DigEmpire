#include "GridMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/MapGrid2D.h"
#include "DigEmpire/Map/CellActor.h"
#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UGridMovementComponent::UGridMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UGridMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!MapComponent) { TryAutoFindMap(); }
    SetupMapReadySubscription();
    if (MapComponent && MapComponent->IsMapReady())
    {
        MoveToFirstFreeCell();
    }
}

// Unsubscribe on EndPlay to avoid stale listeners
void UGridMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (MapReadyHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(MapReadyHandle);
        MapReadyHandle = FGameplayMessageListenerHandle();
    }
    Super::EndPlay(EndPlayReason);
}

void UGridMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!PawnOwner || !UpdatedComponent) return;

	// 1) Read input (AddMovementInput accumulates here).
	const FVector Input3D = ConsumeInputVector();           // arbitrary magnitude
	FVector2D Input2D(Input3D.X, Input3D.Y);
	if (!Input2D.IsNearlyZero())
	{
		Input2D.Normalize();
	}

	// 2) Desired velocity.
	const FVector Desired(Input2D.X * MaxSpeed, Input2D.Y * MaxSpeed, 0.f);

	// 3) Accelerate or brake towards desired.
	const FVector Current(VelocityXY.X, VelocityXY.Y, 0.f);
	FVector NewVel = Current;

	if (!Input2D.IsNearlyZero())
	{
		// Accelerate towards Desired
		const FVector ToDesired = Desired - Current;
		const float MaxAccelThisFrame = Acceleration * DeltaTime;
		const FVector Step = ToDesired.GetClampedToMaxSize(MaxAccelThisFrame);
		NewVel = Current + Step;
	}
	else
	{
		// Brake towards zero
		const float MaxBrakeThisFrame = BrakingDecel * DeltaTime;
		const float Speed = Current.Size();
		if (Speed <= MaxBrakeThisFrame || Speed <= StopSpeedThreshold)
		{
			NewVel = FVector::ZeroVector;
		}
		else
		{
			NewVel = Current - Current.GetSafeNormal() * MaxBrakeThisFrame;
		}
	}

	// Clamp to MaxSpeed just in case
	if (NewVel.SizeSquared() > FMath::Square(MaxSpeed))
	{
		NewVel = NewVel.GetSafeNormal() * MaxSpeed;
	}

	VelocityXY = FVector(NewVel.X, NewVel.Y, 0.f);

	// 4) Apply movement with grid collision.
	const FVector Delta = VelocityXY * DeltaTime;
	if (!Delta.IsNearlyZero())
	{
		TryMoveWithGridCollision(Delta);
	}
}

FVector2D UGridMovementComponent::WorldToGridFloat(const FVector& WorldLocation) const
{
	return FVector2D(WorldLocation.X / TileSize, WorldLocation.Y / TileSize);
}

FVector UGridMovementComponent::GridFloatToWorld(const FVector2D& Grid) const
{
	const float Z = UpdatedComponent ? UpdatedComponent->GetComponentLocation().Z : 0.f;
	return FVector(Grid.X * TileSize, Grid.Y * TileSize, Z);
}

void UGridMovementComponent::TryMoveWithGridCollision(const FVector& Delta)
{
	const FVector Current = UpdatedComponent->GetComponentLocation();
	const FVector Target = Current + Delta;

	// Full move first.
	if (IsWorldPositionValid(Target))
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
		return;
	}

	// Sliding: try X-only, then Y-only.
	const bool MovedX = TryAxisMove(Delta, /*bMoveX=*/true,  /*bMoveY=*/false);
	const bool MovedY = TryAxisMove(Delta, /*bMoveX=*/false, /*bMoveY=*/true);

	if (!MovedX)
	{
		// If X was blocked, zero X velocity
		VelocityXY.X = 0.f;
	}
	if (!MovedY)
	{
		// If Y was blocked, zero Y velocity
		VelocityXY.Y = 0.f;
	}
}

bool UGridMovementComponent::TryAxisMove(const FVector& FullDelta, bool bMoveX, bool bMoveY)
{
	FVector AxisDelta(
		bMoveX ? FullDelta.X : 0.f,
		bMoveY ? FullDelta.Y : 0.f,
		0.f);

	if (AxisDelta.IsNearlyZero()) return false;

	const FVector Current = UpdatedComponent->GetComponentLocation();
	const FVector Target = Current + AxisDelta;

	if (!IsWorldPositionValid(Target))
	{
		return false;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(AxisDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
	return true;
}

bool UGridMovementComponent::IsWorldPositionValid(const FVector& WorldPos) const
{
	if (!MapComponent || !MapComponent->IsMapReady()) return false;

	// Circle center in grid space
	const FVector2D G = WorldToGridFloat(WorldPos);

    // Check all cells whose centers lie within R + 0.5 around G (candidate set).
    const float R = CollisionRadiusCells;           // in cells (circle radius)
    const float Limit = R + 0.5f;                   // candidate radius (cell center to circle center)

	const int32 MinX = FMath::FloorToInt(FMath::FloorToFloat(G.X - Limit));
	const int32 MaxX = FMath::CeilToInt (FMath::CeilToFloat (G.X + Limit));
	const int32 MinY = FMath::FloorToInt(FMath::FloorToFloat(G.Y - Limit));
	const int32 MaxY = FMath::CeilToInt (FMath::CeilToFloat (G.Y + Limit));

	for (int32 y = MinY; y <= MaxY; ++y)
	{
		for (int32 x = MinX; x <= MaxX; ++x)
		{
			if (!InBounds(x, y)) continue;

            if (IsCellBlocked(x, y))
            {
                // Precise circle (center=G, radius=R) vs. axis-aligned cell square [x-0.5,x+0.5]x[y-0.5,y+0.5]
                const float minX = static_cast<float>(x) - 0.5f;
                const float maxX = static_cast<float>(x) + 0.5f;
                const float minY = static_cast<float>(y) - 0.5f;
                const float maxY = static_cast<float>(y) + 0.5f;

                const float cx = FMath::Clamp(G.X, minX, maxX);
                const float cy = FMath::Clamp(G.Y, minY, maxY);
                const float dx = G.X - cx;
                const float dy = G.Y - cy;
                const float dist2 = dx*dx + dy*dy;

                if (dist2 <= R*R)
                {
                    return false;
                }
            }
		}
	}
	return true;
}

bool UGridMovementComponent::IsCellBlocked(int32 GX, int32 GY) const
{
    FMapCell Cell;
    if (MapComponent->GetCell(GX, GY, Cell))
    {
        if (Cell.Occupant)
        {
            return Cell.Occupant->IsBlocked();
        }
    }

	FGameplayTag Obj; int32 Durability = 0;
	if (!MapComponent->GetObjectAt(GX, GY, Obj, Durability))
	{
		return false;
	}
	return (Durability > 0 && Obj.IsValid());
}

bool UGridMovementComponent::InBounds(int32 GX, int32 GY) const
{
	return MapComponent && MapComponent->IsInBounds(GX, GY);
}

void UGridMovementComponent::TryAutoFindMap()
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

void UGridMovementComponent::SetupMapReadySubscription()
{
    if (MapReadyHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(MapReadyHandle);
        MapReadyHandle = FGameplayMessageListenerHandle();
    }

    if (!MapComponent)
    {
        return;
    }

    const FGameplayTag Channel = MapComponent->MapReadyChannel;
    if (!Channel.IsValid())
    {
        return;
    }

    UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
    MapReadyHandle = Bus.RegisterListener<FMapReadyMessage>(
        Channel,
        [this](FGameplayTag /*Tag*/, const FMapReadyMessage& Msg)
        {
            OnMapReadyMessage(Msg);
        });
}

void UGridMovementComponent::OnMapReadyMessage(const FMapReadyMessage& Msg)
{
    if (!Msg.Source)
    {
        return;
    }

    if (!MapComponent)
    {
        MapComponent = Msg.Source;
    }

    if (MapComponent == Msg.Source)
    {
        MoveToFirstFreeCell();
    }
}

void UGridMovementComponent::MoveToFirstFreeCell()
{
    if (!UpdatedComponent || !MapComponent || !MapComponent->IsMapReady())
    {
        return;
    }

    const FIntPoint Size = MapComponent->GetSize();
    for (int32 y = 0; y < Size.Y; ++y)
    {
        for (int32 x = 0; x < Size.X; ++x)
        {
            if (!IsCellBlocked(x, y))
            {
                const FVector TargetWorld = GridFloatToWorld(FVector2D(static_cast<float>(x), static_cast<float>(y)));
                UpdatedComponent->SetWorldLocation(TargetWorld, false, nullptr, ETeleportType::TeleportPhysics);
                return;
            }
        }
    }
}

