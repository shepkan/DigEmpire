#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DigEmpire/Config/DEConstants.h"
#include "GridMovementComponent.generated.h"

class UMapGrid2DComponent;

/**
 * Input-driven (WASD) grid-aware movement:
 * - Continuous world-space motion on XY plane (Z preserved).
 * - Blocks motion when the circle (CollisionRadiusCells) would overlap any occupied cell.
 * - Sliding: if full move is blocked, tries axis-wise (X-only, Y-only).
 * - Uses standard AddMovementInput/ConsumeInputVector path.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UGridMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:
	UGridMovementComponent();

	/** Map provider (auto-found on BeginPlay if null). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid")
	TObjectPtr<UMapGrid2DComponent> MapComponent = nullptr;

	/** World size of one grid cell (read-only; shared project constant). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grid", meta=(ClampMin="1"))
	float TileSize = DEConstants::TileSizeUU;

	/** Collision radius in CELLS (0.0..1.0 ~ within a single cell). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Collision", meta=(ClampMin="0.0"))
	float CollisionRadiusCells = 0.4f;

	/** Max speed in uu/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
	float MaxSpeed = 350.f;

	/** Acceleration towards desired velocity (uu/s^2). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
	float Acceleration = 3000.f;

	/** Deceleration when no input (uu/s^2). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
	float BrakingDecel = 3000.f;

	/** Movement stops when velocity magnitude falls under this (uu/s). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
	float StopSpeedThreshold = 1.f;

	/** Current planar velocity (XY). */
	UPROPERTY(BlueprintReadOnly, Category="Movement")
	FVector VelocityXY = FVector::ZeroVector;

	/** Converts world location to float grid coords (center-of-cell at integers). */
	UFUNCTION(BlueprintPure, Category="Grid|Helpers")
	FVector2D WorldToGridFloat(const FVector& WorldLocation) const;

	/** Converts grid float coords to world location (Z from current component). */
	UFUNCTION(BlueprintPure, Category="Grid|Helpers")
	FVector GridFloatToWorld(const FVector2D& Grid) const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Attempt to move by Delta (XY). Performs grid collision test and sliding. */
    void TryMoveWithGridCollision(const FVector& Delta);

	/** Returns true if circle at WorldPos is NOT overlapping any occupied cell. */
	bool IsWorldPositionValid(const FVector& WorldPos) const;

	/** Returns true if cell has an object (blocked). */
	bool IsCellBlocked(int32 GX, int32 GY) const;

	/** Axis-wise test helper used for sliding. */
	bool TryAxisMove(const FVector& FullDelta, bool bMoveX, bool bMoveY);

    /** Bounds check against map. */
    bool InBounds(int32 GX, int32 GY) const;

    /** Best-effort auto-find of a UMapGrid2DComponent in the world. */
    void TryAutoFindMap();

    /** Subscribe to map-ready event bus and handle immediate readiness. */
    void SetupMapReadySubscription();

    /** Callback for Event Bus payload. */
    void OnMapReadyMessage(const struct FMapReadyMessage& Msg);

    /** Move UpdatedComponent to the first free grid cell (preserving Z). */
    void MoveToFirstFreeCell();

    /** Listener handle for Gameplay Message Subsystem. */
    FGameplayMessageListenerHandle MapReadyHandle;
};
