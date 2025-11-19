#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "DigEmpire/Config/DEConstants.h"
#include "CellActor.generated.h"

/**
 * Base actor that can be placed on a map cell.
 * Override IsBlocked() to control cell accessibility.
 */
UCLASS(Blueprintable)
class ACellActor : public AActor
{
    GENERATED_BODY()

public:
    ACellActor();

    /** Returns true if this actor blocks movement on its cell. */
    UFUNCTION(BlueprintCallable, Category="CellActor")
    virtual bool IsBlocked() const { return false; }

    /** Called when the cell containing this actor becomes visible (first seen). */
    UFUNCTION(BlueprintNativeEvent, Category="CellActor|Events")
    void OnCellSeen();
    virtual void OnCellSeen_Implementation() {}

    /** Called whenever this actor's cell toggles in/out of current player vision. */
    UFUNCTION(BlueprintImplementableEvent, Category="CellActor|Events")
    void OnVisionVisibilityChanged(bool bNowVisible);

    /** Mesh representing this cell object. Visibility is driven by vision. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CellActor")
    TObjectPtr<UStaticMeshComponent> CellMesh = nullptr;

    /** Event Bus channel to listen for current vision updates. Read-only in editor. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CellActor|Vision|Events")
    FGameplayTag VisionChannel;

    /** Tile size in world units used to convert actor location to grid coordinates (read-only; shared project constant). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="CellActor|Vision", meta=(ClampMin="1"))
    float TileSize = DEConstants::TileSizeUU;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Gameplay Message Subsystem listener for vision updates. */
    FGameplayMessageListenerHandle VisionHandle;

    /** Cached visibility state to avoid redundant toggles. */
    bool bCurrentlyVisible = false;

    /** React to current-vision payloads and update mesh visibility. */
    void HandleVisionMessage(const struct FCharacterGridVisionMessage& Msg);
};
