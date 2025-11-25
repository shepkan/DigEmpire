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
class UMapGrid2DComponent;
struct FGridCellWithCoord;

/**
 * Actor that renders a 2D grid using instanced meshes (quads) driven by
 * texture atlases (background and objects). It listens to the "map ready"
 * and update messages and renders when data is available.
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

    /** Listener handle for first-seen messages. */
    FGameplayMessageListenerHandle FirstSeenHandle;

    /** Listener handle for cells-updated messages. */
    FGameplayMessageListenerHandle CellsUpdatedHandle;

    // ===== Atlas rendering (background + object) =====

    /** Unified texture atlas (background + objects). */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TObjectPtr<UTexture2D> AtlasTexture = nullptr;

    /** GameplayTag -> SpriteIndex mapping in the background atlas. */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TMap<FGameplayTag, int32> BackgroundAtlasIndices;

    /** GameplayTag -> SpriteIndex mapping in the atlas. Configure in editor. */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TMap<FGameplayTag, int32> ObjectAtlasIndices;

    /** GameplayTag -> OreIndex mapping (used by material via PerInstanceCustomData[1]). */
    UPROPERTY(EditAnywhere, Category="Rendering|Atlas")
    TMap<FGameplayTag, int32> ObjectOreIndices;

    /** Damage thresholds in percent of initial HP at which DamageDecal stage increases (PerInstanceCustomData[2]). */
    UPROPERTY(EditAnywhere, Category="Rendering|Damage", meta=(ClampMin="0.0", ClampMax="100.0"))
    TArray<float> DamageDecalThresholdsPercent;

    /** Unified HISM used for atlas-based rendering (background + objects). */
    UPROPERTY(Transient)
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> AtlasHISM = nullptr;

    /** Free slots pool for the unified atlas HISM. */
    UPROPERTY(Transient)
    TArray<int32> AtlasFreeSlots;

    /** Cell -> atlas index mapping for background. */
    UPROPERTY(Transient)
    TMap<FIntPoint, int32> BackgroundCellToAtlasIndex;

    /** Cell -> atlas index mapping for objects. */
    UPROPERTY(Transient)
    TMap<FIntPoint, int32> CellToAtlasIndex;

    /** Atlas index -> cell mapping for background. */
    UPROPERTY(Transient)
    TMap<int32, FIntPoint> BackgroundAtlasIndexToCell;

    /** Atlas index -> cell mapping for objects. */
    UPROPERTY(Transient)
    TMap<int32, FIntPoint> AtlasIndexToCell;

    /** Cache of initial object durability per cell for damage % calculations. */
    UPROPERTY(Transient)
    TMap<FIntPoint, int32> InitialObjectDurability;

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

    // ===== Atlas helpers =====
    void EnsureAtlasHISM();
    int32 GetBackgroundAtlasIndex(const FGameplayTag& Tag) const;
    int32 GetObjectAtlasIndex(const FGameplayTag& Tag) const;
    void Atlas_AddOrUpdateBackground(const FGridCellWithCoord& Entry);
    void Atlas_AddOrUpdateObject(const FGridCellWithCoord& Entry);
    void Atlas_RemoveBackgroundAt(const FIntPoint& CellCoord);
    void Atlas_RemoveObjectAt(const FIntPoint& CellCoord);

    /** Resolve Ore index for a given object tag (0 if unmapped). */
    int32 GetOreIndex(const FGameplayTag& Tag) const;

    /** Compute current DamageDecal stage based on thresholds and cached initial durability. */
    int32 ComputeDamageDecalIndex(const FIntPoint& Cell, int32 CurrentDurability) const;

public:
    /** Rebuild rendering for all cells from the map (ignores vision). */
    UFUNCTION(BlueprintCallable, Category="Rendering")
    void RebuildAllFromMap(class UMapGrid2DComponent* InMapSource);
};
