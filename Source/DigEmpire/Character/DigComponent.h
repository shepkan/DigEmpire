#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DigComponent.generated.h"

class UMapGrid2DComponent;
class UGridMovementComponent;

UENUM(BlueprintType)
enum class EDigDirection : uint8
{
    Up,
    Down,
    Left,
    Right
};

/**
 * Digging component: when a direction is engaged, it blocks movement and
 * periodically damages the block one cell away in that direction.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDigComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDigComponent();

    /** Map provider (auto-found on BeginPlay if null). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dig")
    TObjectPtr<UMapGrid2DComponent> MapComponent = nullptr;

    /** Movement component to block while digging (auto-found if null). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dig")
    TObjectPtr<UGridMovementComponent> MovementComponent = nullptr;

    /** World size of one grid cell (read-only; shared project constant). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dig", meta=(ClampMin="1"))
    float TileSize = 100.f; // Will be overridden from constants if available

    /** How often to apply dig damage (seconds). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dig", meta=(ClampMin="0.01"))
    float DigIntervalSeconds = 0.25f;

    /** Damage applied per tick to the object in target cell. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dig", meta=(ClampMin="1"))
    int32 DamagePerTick = 10;

    /** Start digging upwards (Y+1). */
    UFUNCTION(BlueprintCallable, Category="Dig")
    void StartDigUp();

    /** Start digging downwards (Y-1). */
    UFUNCTION(BlueprintCallable, Category="Dig")
    void StartDigDown();

    /** Start digging left (X-1). */
    UFUNCTION(BlueprintCallable, Category="Dig")
    void StartDigLeft();

    /** Start digging right (X+1). */
    UFUNCTION(BlueprintCallable, Category="Dig")
    void StartDigRight();

    /** Stop any ongoing digging and unblock movement. */
    UFUNCTION(BlueprintCallable, Category="Dig")
    void StopDig();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Currently active dig direction (valid only when bIsDigging is true). */
    EDigDirection CurrentDirection = EDigDirection::Right;

    /** Whether we are currently digging. */
    bool bIsDigging = false;

    /** Timer driving periodic dig checks. */
    FTimerHandle DigTimerHandle;

    void TryAutoFindMap();
    void TryAutoFindMovement();

    /** Begin digging in a concrete direction. */
    void StartDigInternal(EDigDirection Dir);

    /** Called every DigIntervalSeconds: apply damage to the target cell if any. */
    void DoDigTick();

    /** Convert owner world location to grid float coords. */
    FVector2D WorldToGridFloat(const FVector& WorldLocation) const;
};

