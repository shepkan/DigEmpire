#include "MapSpriteRenderer.h"

#include "EngineUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "TileTextureSet.h"
#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"

#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
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

    // Pre-create atlas HISM if atlas mode is enabled
    if (IsAtlasEnabled())
    {
        EnsureObjectAtlasHISM();
    }
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

void AMapSpriteRenderer::OnCellsFirstSeen(const FCellsFirstSeenMessage& Msg)
{
    if (!TextureSet) return;

    // For each newly seen cell, render background and object layers
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        const int32 X = Entry.Coord.X;
        const int32 Y = Entry.Coord.Y;

        if (UTexture2D* BgTex = TextureSet->FindBackgroundTexture(Entry.Cell.BackgroundTag))
        {
            if (auto* HISM = GetOrCreateHISMForTexture(BgTex))
            {
                HISM->AddInstance(BuildInstanceTransform(X, Y, BackgroundLayer));
            }
        }

        if (Entry.Cell.HasObject())
        {
            // Cache initial durability for damage-decal computation
            if (!InitialObjectDurability.Contains(Entry.Coord))
            {
                InitialObjectDurability.Add(Entry.Coord, Entry.Cell.ObjectDurability);
            }
            if (IsAtlasEnabled())
            {
                Atlas_AddOrUpdateObject(Entry);
            }
            else
            {
                if (UTexture2D* ObjTex = TextureSet->FindObjectTexture(Entry.Cell.ObjectTag))
                {
                    if (auto* HISM = GetOrCreateHISMForTexture(ObjTex))
                    {
                        const int32 NewIndex = HISM->AddInstance(BuildInstanceTransform(X, Y, ObjectLayer));
                        ObjectInstances.FindOrAdd(Entry.Coord) = { HISM, NewIndex };
                    }
                }
            }
        }
    }
}

void AMapSpriteRenderer::OnCellsUpdated(const FMapCellsUpdatedMessage& Msg)
{
    if (!TextureSet) return;
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
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
        if (IsAtlasEnabled())
        {
            if (Entry.Cell.HasObject())
            {
                Atlas_AddOrUpdateObject(Entry);
            }
            else
            {
                Atlas_RemoveObjectAt(Entry.Coord);
            }
        }
        else
        {
            if (Entry.Cell.HasObject())
            {
                AddOrUpdateObjectInstance(Entry);
            }
            else
            {
                RemoveObjectInstanceAt(Entry.Coord);
            }
        }
    }
}

void AMapSpriteRenderer::AddOrUpdateObjectInstance(const FGridCellWithCoord& Entry)
{
    const int32 X = Entry.Coord.X;
    const int32 Y = Entry.Coord.Y;
    UTexture2D* ObjTex = TextureSet->FindObjectTexture(Entry.Cell.ObjectTag);
    if (!ObjTex) return;

    UHierarchicalInstancedStaticMeshComponent* DesiredHISM = GetOrCreateHISMForTexture(ObjTex);
    if (!DesiredHISM) return;

    if (FInstanceRef* Found = ObjectInstances.Find(Entry.Coord))
    {
        // If HISM is same, nothing to do (we don't update transforms here)
        if (Found->Comp == DesiredHISM)
        {
            return;
        }

        // Otherwise remove from old HISM and re-add to new one
        RemoveObjectInstanceAt(Entry.Coord);
    }

    const int32 NewIndex = DesiredHISM->AddInstance(BuildInstanceTransform(X, Y, ObjectLayer));
    ObjectInstances.FindOrAdd(Entry.Coord) = { DesiredHISM, NewIndex };
}

void AMapSpriteRenderer::RemoveObjectInstanceAt(const FIntPoint& CellCoord)
{
    FInstanceRef Ref;
    if (!ObjectInstances.RemoveAndCopyValue(CellCoord, Ref))
    {
        return;
    }

    if (!Ref.Comp || Ref.Index == INDEX_NONE)
    {
        return;
    }

    // If removing not-last instance, update mapping for the instance that moves into this index
    const int32 Count = Ref.Comp->GetInstanceCount();
    if (Ref.Index >= 0 && Ref.Index < Count - 1)
    {
        FTransform LastXform;
        if (Ref.Comp->GetInstanceTransform(Count - 1, LastXform, /*bWorldSpace=*/true))
        {
            const FVector Loc = LastXform.GetLocation();
            const int32 GX = FMath::RoundToInt(Loc.X / TileSize);
            const int32 GY = FMath::RoundToInt(Loc.Y / TileSize);
            const FIntPoint MovedCell(GX, GY);
            if (FInstanceRef* MovedRef = ObjectInstances.Find(MovedCell))
            {
                // This cell's instance will move into Ref.Index
                MovedRef->Index = Ref.Index;
            }
        }
    }

    Ref.Comp->RemoveInstance(Ref.Index);
}

void AMapSpriteRenderer::EnsureObjectAtlasHISM()
{
    if (ObjectAtlasHISM) return;
    if (!TilePlaneMesh || !TileBaseMaterial || !ObjectAtlasTexture) return;

    const FName CompName = TEXT("HISM_ObjectAtlas");
    ObjectAtlasHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, CompName);
    ObjectAtlasHISM->SetupAttachment(GetRootComponent());
    ObjectAtlasHISM->SetStaticMesh(TilePlaneMesh);
    ObjectAtlasHISM->SetMobility(EComponentMobility::Static);
    ObjectAtlasHISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ObjectAtlasHISM->SetCastShadow(false);
    // PerInstanceCustomData: 0 SpriteIndex, 1 OreIndex, 2 DamageDecal
    ObjectAtlasHISM->NumCustomDataFloats = 3;

    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(TileBaseMaterial, this);
    MID->SetTextureParameterValue(TextureParamName, ObjectAtlasTexture);
    ObjectAtlasHISM->SetMaterial(0, MID);
    ObjectAtlasHISM->RegisterComponent();
}

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
    EnsureObjectAtlasHISM();
    if (!ObjectAtlasHISM) return;

    const int32 AtlasSprite = GetObjectAtlasIndex(Entry.Cell.ObjectTag);
    if (AtlasSprite < 0) return; // unmapped tag
    const int32 OreIdx = GetOreIndex(Entry.Cell.ObjectTag);
    const int32 DamageIdx = ComputeDamageDecalIndex(Entry.Coord, Entry.Cell.ObjectDurability);

    // Existing instance?
    if (int32* ExistingIndex = CellToAtlasIndex.Find(Entry.Coord))
    {
        const int32 Idx = *ExistingIndex;
        // Update custom data (sprite, ore, damage decal)
        ObjectAtlasHISM->SetCustomDataValue(Idx, 0, static_cast<float>(AtlasSprite), true);
        ObjectAtlasHISM->SetCustomDataValue(Idx, 1, static_cast<float>(OreIdx), true);
        ObjectAtlasHISM->SetCustomDataValue(Idx, 2, static_cast<float>(DamageIdx), true);
        // Update transform in place (in case cell repositions)
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        ObjectAtlasHISM->UpdateInstanceTransform(Idx, T, true, true, true);
        return;
    }

    // Allocate new or reuse from pool
    int32 NewIndex = INDEX_NONE;
    if (ObjectFreeSlots.Num() > 0)
    {
        NewIndex = ObjectFreeSlots.Pop(false);
        // Reactivate: set transform to cell and mark visible
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        ObjectAtlasHISM->UpdateInstanceTransform(NewIndex, T, true, true, true);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 1, static_cast<float>(OreIdx), true);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 2, static_cast<float>(DamageIdx), true);
    }
    else
    {
        const FTransform T = BuildInstanceTransform(Entry.Coord.X, Entry.Coord.Y, ObjectLayer);
        NewIndex = ObjectAtlasHISM->AddInstance(T);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 0, static_cast<float>(AtlasSprite), true);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 1, static_cast<float>(OreIdx), true);
        ObjectAtlasHISM->SetCustomDataValue(NewIndex, 2, static_cast<float>(DamageIdx), true);
    }

    CellToAtlasIndex.Add(Entry.Coord, NewIndex);
    AtlasIndexToCell.Add(NewIndex, Entry.Coord);
}

void AMapSpriteRenderer::Atlas_RemoveObjectAt(const FIntPoint& CellCoord)
{
    if (!ObjectAtlasHISM) return;
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
    ObjectAtlasHISM->UpdateInstanceTransform(Index, Hidden, true, true, true);
    // Clear cached durability for this cell
    InitialObjectDurability.Remove(CellCoord);

    ObjectFreeSlots.Add(Index);
}

void AMapSpriteRenderer::ClearAll()
{
    for (auto& Pair : TextureToHISM)
    {
        if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	TextureToHISM.Empty();

    ObjectInstances.Empty();

    if (ObjectAtlasHISM)
    {
        ObjectAtlasHISM->DestroyComponent();
        ObjectAtlasHISM = nullptr;
    }
    ObjectFreeSlots.Empty();
    CellToAtlasIndex.Empty();
    AtlasIndexToCell.Empty();
}
UHierarchicalInstancedStaticMeshComponent* AMapSpriteRenderer::GetOrCreateHISMForTexture(UTexture2D* Texture)
{
	if (!Texture) return nullptr;
	if (!TilePlaneMesh || !TileBaseMaterial) return nullptr;

	if (TObjectPtr<UHierarchicalInstancedStaticMeshComponent>* Found = TextureToHISM.Find(Texture))
	{
		return *Found;
	}

	const FName CompName = *FString::Printf(TEXT("HISM_%s"), *GetNameSafe(Texture));
	auto* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, CompName);
	HISM->SetupAttachment(GetRootComponent());
	HISM->SetStaticMesh(TilePlaneMesh);
	HISM->SetMobility(EComponentMobility::Static);
	HISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HISM->SetCastShadow(false);

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(TileBaseMaterial, this);
	MID->SetTextureParameterValue(TextureParamName, Texture);
	HISM->SetMaterial(0, MID);

	HISM->RegisterComponent();
	TextureToHISM.Add(Texture, HISM);
	return HISM;
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
