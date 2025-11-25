#include "MapSpriteRenderer.h"

#include "EngineUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"

#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/BusEvents/LuminanceMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"

AMapSpriteRenderer::AMapSpriteRenderer()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root scene to attach HISM components under it
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    // HISM components are Static; parent must be Static too
    Root->SetMobility(EComponentMobility::Static);

    // Default event channel for first-seen cells
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;
    CellsUpdatedChannel = TAG_Map_CellsUpdated;
    LuminanceChannel = TAG_Render_LuminanceUpdate;
}

void AMapSpriteRenderer::BeginPlay()
{
    Super::BeginPlay();
    
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;

    // Try to auto-pick a map component if none assigned (optional for event-driven)
    TryAutoFindMapComponent();

    // Subscribe to first-seen cell messages
    SetupFirstSeenSubscription();

    // Subscribe to cells-updated to reflect object changes
    SetupCellsUpdatedSubscription();

    // Subscribe to luminance updates
    SetupLuminanceSubscription();

    // Pre-create unified atlas HISM if texture is configured
    EnsureAtlasHISM();
}

void AMapSpriteRenderer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unregister listener if it was registered
    if (FirstSeenHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(FirstSeenHandle);
    }
    if (CellsUpdatedHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(CellsUpdatedHandle);
    }
    if (LuminanceHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(LuminanceHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void AMapSpriteRenderer::TryAutoFindMapComponent()
{
	if (MapSource) return;

	// Simple auto-find: first valid UMapGrid2DComponent in the world
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (UMapGrid2DComponent* Comp = It->FindComponentByClass<UMapGrid2DComponent>())
		{
			MapSource = Comp;
			break;
		}
	}
}

void AMapSpriteRenderer::SetupFirstSeenSubscription()
{
    if (!FirstSeenChannel.IsValid())
    {
        return;
    }

    UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
    FirstSeenHandle = Bus.RegisterListener<FCellsFirstSeenMessage>(
        FirstSeenChannel,
        [this](FGameplayTag, const FCellsFirstSeenMessage& Msg)
        {
            OnCellsFirstSeen(Msg);
        });
}

void AMapSpriteRenderer::SetupCellsUpdatedSubscription()
{
    if (!CellsUpdatedChannel.IsValid())
    {
        return;
    }

    UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
    CellsUpdatedHandle = Bus.RegisterListener<FMapCellsUpdatedMessage>(
        CellsUpdatedChannel,
        [this](FGameplayTag, const FMapCellsUpdatedMessage& Msg)
        {
            OnCellsUpdated(Msg);
        });
}

void AMapSpriteRenderer::SetupLuminanceSubscription()
{
    if (!LuminanceChannel.IsValid())
    {
        return;
    }
    UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
    LuminanceHandle = Bus.RegisterListener<FLuminanceUpdateMessage>(
        LuminanceChannel,
        [this](FGameplayTag, const FLuminanceUpdateMessage& Msg)
        {
            OnLuminanceUpdate(Msg);
        });
}

void AMapSpriteRenderer::OnCellsFirstSeen(const FCellsFirstSeenMessage& Msg)
{
    // Atlas-only path; require atlas texture
    if (!AtlasTexture) return;

    // For each newly seen cell, render background and object layers
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        const int32 X = Entry.Coord.X;
        const int32 Y = Entry.Coord.Y;

        Atlas_AddOrUpdateBackground(Entry);

        if (Entry.Cell.HasObject())
        {
            // Cache initial durability for damage-decal computation
            if (!InitialObjectDurability.Contains(Entry.Coord))
            {
                InitialObjectDurability.Add(Entry.Coord, Entry.Cell.ObjectDurability);
            }
            Atlas_AddOrUpdateObject(Entry);
        }
    }
}

void AMapSpriteRenderer::OnCellsUpdated(const FMapCellsUpdatedMessage& Msg)
{
    if (!AtlasTexture) return;
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        // Update background in case it changed
        Atlas_AddOrUpdateBackground(Entry);
        // Maintain initial durability cache for damage-decal computation
        if (Entry.Cell.HasObject())
        {
            int32* InitPtr = InitialObjectDurability.Find(Entry.Coord);
            if (!InitPtr || Entry.Cell.ObjectDurability > *InitPtr || *InitPtr <= 0)
            {
                InitialObjectDurability.Add(Entry.Coord, Entry.Cell.ObjectDurability);
            }
        }
        else
        {
            InitialObjectDurability.Remove(Entry.Coord);
        }
        if (Entry.Cell.HasObject())
        {
            Atlas_AddOrUpdateObject(Entry);
        }
        else
        {
            Atlas_RemoveObjectAt(Entry.Coord);
        }
    }
}

void AMapSpriteRenderer::EnsureAtlasHISM()
{
    if (AtlasHISM) return;
    if (!TilePlaneMesh || !TileBaseMaterial || !AtlasTexture) return;

    const FName CompName = TEXT("HISM_Atlas");
    AtlasHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, CompName);
    AtlasHISM->SetupAttachment(GetRootComponent());
    AtlasHISM->SetStaticMesh(TilePlaneMesh);
    AtlasHISM->SetMobility(EComponentMobility::Static);
    AtlasHISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AtlasHISM->SetCastShadow(false);
    // PerInstanceCustomData: 0 SpriteIndex (both), 1 OreIndex (objects), 2 DamageDecal (objects), 3 Luminance
    AtlasHISM->NumCustomDataFloats = 4;

    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(TileBaseMaterial, this);
    MID->SetTextureParameterValue(TextureParamName, AtlasTexture);
    AtlasHISM->SetMaterial(0, MID);
    AtlasHISM->RegisterComponent();
}

void AMapSpriteRenderer::OnLuminanceUpdate(const FLuminanceUpdateMessage& Msg)
{
    if (!AtlasHISM) return;

    auto ApplyToCell = [this](const FIntPoint& Cell, float Value)
    {
        if (int32* BgIdx = BackgroundCellToAtlasIndex.Find(Cell))
        {
            AtlasHISM->SetCustomDataValue(*BgIdx, 3, Value, true);
        }
        if (int32* ObjIdx = CellToAtlasIndex.Find(Cell))
        {
            AtlasHISM->SetCustomDataValue(*ObjIdx, 3, Value, true);
        }
    };

    // Apply luminance per visible ring
    for (int32 RingIdx = 0; RingIdx < Msg.RadiusLayers.Num(); ++RingIdx)
    {
        const float Lum = Msg.LuminanceByRing.IsValidIndex(RingIdx) ? Msg.LuminanceByRing[RingIdx] : 1.0f;
        const FGridCellsRing& Ring = Msg.RadiusLayers[RingIdx];
        for (const FGridCellWithCoord& Entry : Ring.Cells)
        {
            ApplyToCell(Entry.Coord, Lum);
        }
    }

    // Reset luminance for cells that left visibility
    for (const FIntPoint& Cell : Msg.CellsLeftVisibility)
    {
        ApplyToCell(Cell, DefaultLuminance);
    }
}

int32 AMapSpriteRenderer::GetBackgroundAtlasIndex(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid()) return -1;
    if (const int32* Found = BackgroundAtlasIndices.Find(Tag))
    {
        return *Found;
    }
    return -1;
}

void AMapSpriteRenderer::Atlas_AddOrUpdateBackground(const FGridCellWithCoord& Entry)
{
    EnsureAtlasHISM();
    if (!AtlasHISM) return;

    const int32 AtlasSprite = GetBackgroundAtlasIndex(Entry.Cell.BackgroundTag);
    if (AtlasSprite < 0) return; // unmapped

    if (int32* ExistingIndex = BackgroundCellToAtlasIndex.Find(Entry.Coord))
    {
        const int32 Idx = *ExistingIndex;
        AtlasHISM->SetCustomDataValue(Idx, 0, static_cast<float>(AtlasSprite), true);
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, BackgroundLayer);
        AtlasHISM->UpdateInstanceTransform(Idx, T, true, true, true);
        return;
    }

    int32 NewIndex = INDEX_NONE;
    if (AtlasFreeSlots.Num() > 0)
    {
        NewIndex = AtlasFreeSlots.Pop(false);
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, BackgroundLayer);
        AtlasHISM->UpdateInstanceTransform(NewIndex, T, true, true, true);
        AtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
    }
    else
    {
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, BackgroundLayer);
        NewIndex = AtlasHISM->AddInstance(T);
        AtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
    }

    BackgroundCellToAtlasIndex.Add(Entry.Coord, NewIndex);
    BackgroundAtlasIndexToCell.Add(NewIndex, Entry.Coord);
}

void AMapSpriteRenderer::Atlas_RemoveBackgroundAt(const FIntPoint& CellCoord)
{
    if (!AtlasHISM) return;
    int32 Index;
    if (!BackgroundCellToAtlasIndex.RemoveAndCopyValue(CellCoord, Index))
    {
        return;
    }
    BackgroundAtlasIndexToCell.Remove(Index);
    FTransform Hidden = BuildInstanceTransform(CellCoord.X, CellCoord.Y, BackgroundLayer);
    Hidden.AddToTranslation(FVector(0, 0, -100000.f));
    Hidden.SetScale3D(FVector(0.001f));
    AtlasHISM->UpdateInstanceTransform(Index, Hidden, true, true, true);
    AtlasFreeSlots.Add(Index);
}

// Per-texture object instance path removed (atlas-only)

// Unified atlas HISM; object path uses same component

int32 AMapSpriteRenderer::GetObjectAtlasIndex(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid()) return -1;
    if (const int32* Found = ObjectAtlasIndices.Find(Tag))
    {
        return *Found;
    }
    return -1;
}

int32 AMapSpriteRenderer::GetOreIndex(const FGameplayTag& Tag) const
{
    if (!Tag.IsValid()) return 0;
    if (const int32* Found = ObjectOreIndices.Find(Tag))
    {
        return *Found;
    }
    return 0;
}

int32 AMapSpriteRenderer::ComputeDamageDecalIndex(const FIntPoint& Cell, int32 CurrentDurability) const
{
    const int32* InitPtr = InitialObjectDurability.Find(Cell);
    if (!InitPtr || *InitPtr <= 0)
    {
        return 0;
    }
    const int32 Init = *InitPtr;
    const int32 DamageTaken = FMath::Max(0, Init - CurrentDurability);
    const float DamagePercent = (Init > 0) ? (static_cast<float>(DamageTaken) / static_cast<float>(Init)) * 100.f : 0.f;

    int32 Stage = 0;
    for (float Th : DamageDecalThresholdsPercent)
    {
        if (DamagePercent >= Th)
        {
            ++Stage;
        }
    }
    return Stage;
}

void AMapSpriteRenderer::Atlas_AddOrUpdateObject(const FGridCellWithCoord& Entry)
{
    EnsureAtlasHISM();
    if (!AtlasHISM) return;

    const int32 AtlasSprite = GetObjectAtlasIndex(Entry.Cell.ObjectTag);
    if (AtlasSprite < 0) return; // unmapped tag
    const int32 OreIdx = GetOreIndex(Entry.Cell.OreTag);
    const int32 DamageIdx = ComputeDamageDecalIndex(Entry.Coord, Entry.Cell.ObjectDurability);

    // Existing instance?
    if (int32* ExistingIndex = CellToAtlasIndex.Find(Entry.Coord))
    {
        const int32 Idx = *ExistingIndex;
        // Update custom data (sprite, ore, damage decal)
        AtlasHISM->SetCustomDataValue(Idx, 0, static_cast<float>(AtlasSprite), true);
        AtlasHISM->SetCustomDataValue(Idx, 1, static_cast<float>(OreIdx), true);
        AtlasHISM->SetCustomDataValue(Idx, 2, static_cast<float>(DamageIdx), true);
        // Update transform in place (in case cell repositions)
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        AtlasHISM->UpdateInstanceTransform(Idx, T, true, true, true);
        return;
    }

    // Allocate new or reuse from pool
    int32 NewIndex = INDEX_NONE;
    if (AtlasFreeSlots.Num() > 0)
    {
        NewIndex = AtlasFreeSlots.Pop(false);
        // Reactivate: set transform to cell and mark visible
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        AtlasHISM->UpdateInstanceTransform(NewIndex, T, true, true, true);
        AtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
        AtlasHISM->SetCustomDataValue(NewIndex, 1, static_cast<float>(OreIdx), true);
        AtlasHISM->SetCustomDataValue(NewIndex, 2, static_cast<float>(DamageIdx), true);
    }
    else
    {
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        NewIndex = AtlasHISM->AddInstance(T);
        AtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
        AtlasHISM->SetCustomDataValue(NewIndex, 1, static_cast<float>(OreIdx), true);
        AtlasHISM->SetCustomDataValue(NewIndex, 2, static_cast<float>(DamageIdx), true);
    }

    CellToAtlasIndex.Add(Entry.Coord, NewIndex);
    AtlasIndexToCell.Add(NewIndex, Entry.Coord);
}

void AMapSpriteRenderer::Atlas_RemoveObjectAt(const FIntPoint& CellCoord)
{
    if (!AtlasHISM) return;
    int32 Index;
    if (!CellToAtlasIndex.RemoveAndCopyValue(CellCoord, Index))
    {
        return;
    }
    AtlasIndexToCell.Remove(Index);

    // Deactivate in pool: hide offscreen
    const FVector HiddenLoc(1.0e7f, 1.0e7f, -1.0e7f);
    FTransform Hidden = FTransform::Identity;
    Hidden.SetLocation(HiddenLoc);
    Hidden.SetScale3D(FVector(0.001f));
    AtlasHISM->UpdateInstanceTransform(Index, Hidden, true, true, true);
    // Clear cached durability for this cell
    InitialObjectDurability.Remove(CellCoord);

    AtlasFreeSlots.Add(Index);
}

void AMapSpriteRenderer::ClearAll()
{
    if (AtlasHISM)
    {
        AtlasHISM->DestroyComponent();
        AtlasHISM = nullptr;
    }
    AtlasFreeSlots.Empty();
    CellToAtlasIndex.Empty();
    AtlasIndexToCell.Empty();
    BackgroundCellToAtlasIndex.Empty();
    BackgroundAtlasIndexToCell.Empty();
}

FTransform AMapSpriteRenderer::BuildInstanceTransform(int32 GridX, int32 GridY, int32 LayerIndex) const
{
	const float X = static_cast<float>(GridX) * TileSize;
	const float Y = static_cast<float>(GridY) * TileSize;
	const float Z = ZBaseOffset + (static_cast<float>(LayerIndex) * LayerStep);

	const float Scale = TileSize / 100.f; // default plane is 100x100
	FTransform T;
	T.SetLocation(FVector(X, Y, Z));
	T.SetRotation(FQuat::Identity);
	T.SetScale3D(FVector(Scale, Scale, 1.f));
	return T;
}

void AMapSpriteRenderer::RebuildAllFromMap(UMapGrid2DComponent* InMapSource)
{
    if (InMapSource)
    {
        MapSource = InMapSource;
    }
    if (!MapSource || !MapSource->IsMapReady() || !AtlasTexture)
    {
        return;
    }

    ClearAll();
    EnsureAtlasHISM();

    const FIntPoint Size = MapSource->GetSize();
    FGridCellWithCoord Entry;
    for (int32 y = 0; y < Size.Y; ++y)
    {
        for (int32 x = 0; x < Size.X; ++x)
        {
            Entry.Coord = FIntPoint(x, y);
            if (!MapSource->GetCell(x, y, Entry.Cell)) continue;

            Atlas_AddOrUpdateBackground(Entry);

            if (Entry.Cell.HasObject())
            {
                if (!InitialObjectDurability.Contains(Entry.Coord))
                {
                    InitialObjectDurability.Add(Entry.Coord, Entry.Cell.ObjectDurability);
                }

                Atlas_AddOrUpdateObject(Entry);
            }
        }
    }
}
