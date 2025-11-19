// CharacterGridVisionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DigEmpire/Config/DEConstants.h"
#include "CharacterGridVisionComponent.generated.h"

class UMapGrid2DComponent;

/**
 * Periodically samples owner world position, converts to grid,
 * gathers nearby cells within radius and publishes via Gameplay Message Bus.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UCharacterGridVisionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterGridVisionComponent();

    /** Map provider (auto-found on BeginPlay if null). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vision")
    TObjectPtr<UMapGrid2DComponent> MapComponent = nullptr;

    /** World size of one grid cell (read-only; shared project constant). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vision", meta=(ClampMin="1"))
    float TileSize = DEConstants::TileSizeUU;

    /** Vision radius in cells (euclidean). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vision", meta=(ClampMin="0"))
    int32 VisionRadiusCells = 3;

    /** How often to publish vision updates (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vision", meta=(ClampMin="0.01"))
    float VisionIntervalSeconds = 0.25f;

    /** Event Bus channel to publish vision messages (read-only in editor). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vision|Events")
    FGameplayTag VisionChannel;

    /** Event Bus channel to publish cells that became viewed for the first time (read-only in editor). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Vision|Events")
    FGameplayTag FirstSeenChannel;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Timer to trigger periodic sampling. */
    FTimerHandle VisionTimerHandle;

    /** Best-effort auto-find of a UMapGrid2DComponent in the world. */
    void TryAutoFindMap();

    /** Timer callback: read owner position, gather cells, publish. */
    void DoVisionTick();

    /** Helper: convert world location to grid float coords. */
    FVector2D WorldToGridFloat(const FVector& WorldLocation) const;
};
