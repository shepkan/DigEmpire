#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DigEmpire/Config/DEConstants.h"
#include "MapSpriteRenderer.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class UTexture2D;
class UStaticMesh;
class UTileTextureSet;
class UMapGrid2DComponent;
struct FGridCellWithCoord;

/** Mapping from grid cell to HISM + instance index for object layer. */
USTRUCT()
struct FInstanceRef
{
	GENERATED_BODY()
	
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Comp = nullptr;
    int32 Index = INDEX_NONE;
};

/**
 * Actor that renders a 2D grid using instanced meshes (quads) and textures
 * resolved from a TileTextureSet. It listens to the "map ready" message and
 * renders when the map is available.
 */
UCLASS(BlueprintType, Blueprintable)
class AMapSpriteRenderer : public AActor
{
    GENERATED_BODY()

public:
	AMapSpriteRenderer();

	/** Optional direct reference to the map component. If not set, the renderer will try to auto-find one in the world on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map")
	TObjectPtr<UMapGrid2DComponent> MapSource = nullptr;

    /** Event Bus channel to listen for first-seen cells (must match the publisher). Read-only in editor. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map|Events")
    FGameplayTag FirstSeenChannel;

	/** Data asset with textures for backgrGound/object tags. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	TObjectPtr<UTileTextureSet> TextureSet = nullptr;

	/** Base quad mesh (e.g., /Engine/BasicShapes/Plane). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	TObjectPtr<UStaticMesh> TilePlaneMesh;

	/** Base material that exposes a texture parameter (TextureParamName). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	TObjectPtr<UMaterialInterface> TileBaseMaterial;

	/** Name of the texture parameter in the material. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	FName TextureParamName = TEXT("SpriteTexture");

	/** Tile size in world units (read-only; shared project constant). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rendering", meta=(ClampMin="1"))
    float TileSize = DEConstants::TileSizeUU;

	/** Z step per layer (final Z = Layer * LayerStep + ZBaseOffset). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	float LayerStep = 10.f;

	/** Additional Z offset for all tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	float ZBaseOffset = 0.f;

	/** Layer used for background tiles. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	int32 BackgroundLayer = 0;

	/** Layer used for object tiles (must be different from background to avoid z-fighting). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering")
	int32 ObjectLayer = 1;

	/** Event Bus channel for cells-updated messages. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map|Events")
	FGameplayTag CellsUpdatedChannel;

    // No full rebuild; rendering is event-driven.

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Per-texture HISM cache to avoid creating a new component per instance. Key = Texture pointer. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UTexture2D>, TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> TextureToHISM;

    /** Listener handle for first-seen messages. */
    FGameplayMessageListenerHandle FirstSeenHandle;

    /** Listener handle for cells-updated messages. */
    FGameplayMessageListenerHandle CellsUpdatedHandle;

    UPROPERTY(Transient)
    TMap<FIntPoint, FInstanceRef> ObjectInstances;

    // ===== Atlas + Pool path for object layer =====
    /** If true, render object layer via a single atlas HISM with per-instance custom data. */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    bool bUseObjectAtlas = false;

    /** Texture atlas for objects. Must match TileBaseMaterial expectations. */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TObjectPtr<UTexture2D> ObjectAtlasTexture = nullptr;

    /** GameplayTag -> SpriteIndex mapping in the atlas. Configure in editor. */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TMap<FGameplayTag, int32> ObjectAtlasIndices;

    /** HISM used for atlas-based object rendering. */
    UPROPERTY(Transient)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> ObjectAtlasHISM = nullptr;

    /** Free slots pool for atlas HISM. */
    UPROPERTY(Transient)
    TArray<int32> ObjectFreeSlots;

    /** Cell -> atlas index mapping. */
    UPROPERTY(Transient)
    TMap<FIntPoint, int32> CellToAtlasIndex;

    /** Atlas index -> cell mapping. */
    UPROPERTY(Transient)
    TMap<int32, FIntPoint> AtlasIndexToCell;

	/** Find or create a HISM for a given texture (creates a MID with that texture). */
	UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISMForTexture(UTexture2D* Texture);

	/** Build transform for a tile at grid (X,Y) placed on a given layer index. */
	FTransform BuildInstanceTransform(int32 GridX, int32 GridY, int32 LayerIndex) const;

    /** Subscribe to Event Bus for first-seen cells. */
    void SetupFirstSeenSubscription();
    void SetupCellsUpdatedSubscription();

	/** Try to auto-locate a map component in the world if not assigned. */
	void TryAutoFindMapComponent();

	/** Clear all instances/components generated so far. */
    void ClearAll();

    /** Handle first-seen payload. */
    void OnCellsFirstSeen(const struct FCellsFirstSeenMessage& Msg);

    /** Handle cells updated payload. */
    void OnCellsUpdated(const struct FMapCellsUpdatedMessage& Msg);

    /** Ensure an object instance exists (and matches current tag/texture) for this cell. */
    void AddOrUpdateObjectInstance(const FGridCellWithCoord& Entry);

    /** Remove object instance for this cell if it exists. */
    void RemoveObjectInstanceAt(const FIntPoint& CellCoord);

    // ===== Atlas helpers =====
    bool IsAtlasEnabled() const { return bUseObjectAtlas && ObjectAtlasTexture != nullptr; }
    void EnsureObjectAtlasHISM();
    int32 GetObjectAtlasIndex(const FGameplayTag& Tag) const;
    void Atlas_AddOrUpdateObject(const FGridCellWithCoord& Entry);
    void Atlas_RemoveObjectAt(const FIntPoint& CellCoord);
};
