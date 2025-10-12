#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "MapSpriteRenderer.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class UTexture2D;
class UStaticMesh;
class UTileTextureSet;
class UMapGrid2DComponent;

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

	/** Event Bus channel to listen to when the map is ready (must match the publisher). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Map|Events")
	FGameplayTag MapReadyChannel;

	/** Data asset with textures for background/object tags. */
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

	/** Tile size in world units (plane default is 100x100 uu). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rendering", meta=(ClampMin="1"))
	float TileSize = 100.f;

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

	/** Rebuild the whole map immediately (if MapSource is valid and ready). */
	UFUNCTION(BlueprintCallable, Category="Rendering")
	void RebuildNow();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Per-texture HISM cache to avoid creating a new component per instance. Key = Texture pointer. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UTexture2D>, TObjectPtr<UHierarchicalInstancedStaticMeshComponent>> TextureToHISM;

	/** Listener handle for Gameplay Message Subsystem. */
	FGameplayMessageListenerHandle MapReadyHandle;

	/** Find or create a HISM for a given texture (creates a MID with that texture). */
	UHierarchicalInstancedStaticMeshComponent* GetOrCreateHISMForTexture(UTexture2D* Texture);

	/** Build transform for a tile at grid (X,Y) placed on a given layer index. */
	FTransform BuildInstanceTransform(int32 GridX, int32 GridY, int32 LayerIndex) const;

	/** Subscribe to Event Bus for map-ready; also do a one-shot ready check. */
	void SetupMapReadySubscription();

	/** Try to auto-locate a map component in the world if not assigned. */
	void TryAutoFindMapComponent();

	/** Clear all instances/components generated so far. */
	void ClearAll();

	/** Internal full rebuild using MapSource + TextureSet. */
	void RebuildInternal();

	/** Callback for Event Bus payload. */
	void OnMapReadyMessage(const struct FMapReadyMessage& Msg);
};
