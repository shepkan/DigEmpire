// MapGrid2DComponent.cpp
#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"
#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Generation/ZoneBorderGenerator.h"
#include "Generation/ZonePassageGenerator.h"
#include "Zones/MapZoneGenerator.h"
#include "Zones/ZoneGenSettings.h"
#include "Rooms/RoomGenerator.h"
#include "Rooms/RoomGenSettings.h"
#include "Generation/CaveGenerator.h"
#include "Generation/CaveGenSettings.h"
#include "Generation/ZoneConnectivityFixer.h"
#include "Generation/ZoneDoorPlacer.h"
#include "Generation/ZoneDoorSettings.h"

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
	//BuildBorder();

	UMapZoneGenerator* Gen = NewObject<UMapZoneGenerator>();
	TArray<int32> ZoneLabels;
	const bool bOk = Gen->Generate(MapInstance, ZoneSettings, GetWorld(), ZoneLabels);

	if (bOk)
	{
		// Persist zone labels into the map cells
		MapInstance->ApplyZoneLabels(ZoneLabels);
        UZoneBorderGenerator* BorderGen = NewObject<UZoneBorderGenerator>();
        if (BorderGen->Generate(MapInstance, ZoneLabels, BorderSettings))
        {
            // After walls, carve passages and store them on the map
            UZonePassageGenerator* PassageGen = NewObject<UZonePassageGenerator>();
            if (PassageGen->Generate(MapInstance, ZoneLabels, BorderSettings))
            {
                MapInstance->SetPassages(PassageGen->GetPassages());
            }
        }

		// Optional: place rooms inside zones using RoomSettings
		if (RoomSettings)
		{
			URoomGenerator* RoomGen = NewObject<URoomGenerator>();
			RoomGen->Generate(MapInstance, ZoneLabels, RoomSettings, BorderSettings);
		}

		// Optional: run per-zone cave cellular automata after rooms
		if (CaveSettings)
		{
			UCaveGenerator* CaveGen = NewObject<UCaveGenerator>();
			CaveGen->Generate(MapInstance, ZoneLabels, CaveSettings, BorderSettings);
		}

		// (debug visualization removed)

		// Connect open areas inside each zone to the largest area using minimal carving
		{
			UZoneConnectivityFixer* Fixer = NewObject<UZoneConnectivityFixer>();
			Fixer->Generate(MapInstance, ZoneLabels, BorderSettings);
		}

        // Place doors at the center of each passage if configured
        if (DoorSettings && DoorSettings->DoorClass)
        {
            UZoneDoorPlacer* DoorPlacer = NewObject<UZoneDoorPlacer>();
            DoorPlacer->Generate(MapInstance, DoorSettings, GetWorld());
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
