// MapGrid2DComponent.cpp
#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"
#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Generation/MapGenerationStepDataBase.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"

UMapGrid2DComponent::UMapGrid2DComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMapGrid2DComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bInitializeOnBeginPlay)
	{
		InitializeAndBuild();
	}
}

bool UMapGrid2DComponent::IsMapReady() const
{
	if (!MapInstance) return false;
	const FIntPoint Size = MapInstance->GetSize();
	return (Size.X > 0 && Size.Y > 0);
}

FIntPoint UMapGrid2DComponent::GetSize() const
{
	return IsMapReady() ? MapInstance->GetSize() : FIntPoint::ZeroValue;
}

bool UMapGrid2DComponent::IsInBounds(int32 X, int32 Y) const
{
	return IsMapReady() ? MapInstance->IsInBounds(X, Y) : false;
}

bool UMapGrid2DComponent::GetBackgroundAt(int32 X, int32 Y, FGameplayTag& OutBackgroundTag) const
{
	return IsMapReady() ? MapInstance->GetBackgroundAt(X, Y, OutBackgroundTag) : false;
}

bool UMapGrid2DComponent::GetObjectAt(int32 X, int32 Y, FGameplayTag& OutObjectTag, int32& OutDurability) const
{
    return IsMapReady() ? MapInstance->GetObjectAt(X, Y, OutObjectTag, OutDurability) : false;
}

bool UMapGrid2DComponent::SetActorAt(int32 X, int32 Y, ACellActor* InActor)
{
    return IsMapReady() ? MapInstance->SetActorAt(X, Y, InActor) : false;
}

ACellActor* UMapGrid2DComponent::GetActorAt(int32 X, int32 Y) const
{
    return IsMapReady() ? MapInstance->GetActorAt(X, Y) : nullptr;
}

bool UMapGrid2DComponent::GetCell(int32 X, int32 Y, FMapCell& OutCell) const
{
    return IsMapReady() ? MapInstance->GetCell(X, Y, OutCell) : false;
}

bool UMapGrid2DComponent::DamageObjectAt(int32 X, int32 Y, int32 Damage, bool& bOutDestroyed)
{
    bOutDestroyed = false;
    if (!IsMapReady() || !MapInstance || Damage <= 0 || !IsInBounds(X, Y))
    {
        return false;
    }

    FGameplayTag Obj; int32 Dur = 0;
    if (!MapInstance->GetObjectAt(X, Y, Obj, Dur))
    {
        return false; // no object to damage
    }

    int32 NewDur = Dur - Damage;
    if (NewDur <= 0)
    {
        MapInstance->RemoveObjectAt(X, Y);
        bOutDestroyed = true;
    }
    else
    {
        MapInstance->AddOrUpdateObjectAt(X, Y, Obj, NewDur);
    }

    // Prepare and broadcast update
    FGridCellWithCoord Entry;
    Entry.Coord = FIntPoint(X, Y);
    MapInstance->GetCell(X, Y, Entry.Cell);
    TArray<FGridCellWithCoord> Cells;
    Cells.Add(Entry);
    BroadcastCellsUpdated(Cells);
    return true;
}

void UMapGrid2DComponent::InitializeAndBuild()
{
	// Create the map instance if missing.
	if (!MapInstance)
	{
		MapInstance = NewObject<UMapGrid2D>(this);
	}

	// Initialize size.
	const int32 SafeSizeX = FMath::Max(1, MapSizeX);
	const int32 SafeSizeY = FMath::Max(1, MapSizeY);
	MapInstance->Initialize(SafeSizeX, SafeSizeY);

    // Fill and build borders.
    FillBackground();

    // Execute configured generation steps in order
    TArray<int32> ZoneLabels;
    for (const UMapGenerationStepDataBase* Step : GenerationSteps)
    {
        if (Step)
        {
            Step->ExecuteGenerationStep(MapInstance, GetWorld(), ZoneLabels);
        }
    }

    // Notify via Event Bus.
    BroadcastMapReady();
}


	void UMapGrid2DComponent::FillBackground()
{
	if (!IsMapReady()) return;

	const FIntPoint Size = MapInstance->GetSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			MapInstance->SetBackgroundAt(X, Y, DefaultBackgroundTag);
		}
	}
}

void UMapGrid2DComponent::BroadcastMapReady()
{
	// If no channel is provided, do nothing silently.
	if (!MapReadyChannel.IsValid() || !IsMapReady())
	{
		return;
	}

	FMapReadyMessage Message;
	Message.Source = this;
	Message.Size = MapInstance->GetSize();

	UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
	Bus.BroadcastMessage(MapReadyChannel, Message);
}

void UMapGrid2DComponent::SetZoneDepths(const TArray<int32>& Depths)
{
    ZoneInfos.SetNum(Depths.Num());
    for (int32 i = 0; i < Depths.Num(); ++i)
    {
        ZoneInfos[i].Depth = Depths[i];
    }
}

void UMapGrid2DComponent::BroadcastCellsUpdated(const TArray<FGridCellWithCoord>& Cells)
{
    if (Cells.Num() == 0) return;
    FMapCellsUpdatedMessage Msg;
    Msg.Source = this;
    Msg.Cells = Cells;
    UGameplayMessageSubsystem::Get(this).BroadcastMessage(TAG_Map_CellsUpdated, Msg);
}
