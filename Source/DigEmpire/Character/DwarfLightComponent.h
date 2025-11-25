// DwarfLightComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DwarfLightComponent.generated.h"

class UDwarfLightSettings;
class UCharacterGridVisionComponent;

/**
 * Tracks dwarf's LightPower, decreases over time and updates
 * vision radius thresholds on the owner's UCharacterGridVisionComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDwarfLightComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDwarfLightComponent();

    /** Settings asset controlling defaults, decay and thresholds. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
    TObjectPtr<UDwarfLightSettings> Settings = nullptr;

    /** Current light power [0..100], read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Light", meta=(ClampMin="0", ClampMax="100"))
    float LightPower = 0.f;

    /** Enable/disable automatic decay over time. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light")
    bool bDecayEnabled = true;


protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    /** Cached pointer to owner's vision component, if any. */
    TWeakObjectPtr<UCharacterGridVisionComponent> CachedVision;

    /** Applies threshold mapping and updates vision radius if changed. */
    void ApplyVisionFromLight();

    /** Returns target vision radius for current LightPower based on Settings. */
    int32 ComputeVisionRadiusFor(float Power) const;

    /** Last applied vision radius used to detect threshold crossings. */
    int32 LastAppliedVisionRadius = INDEX_NONE;

    /** Writes the given radius to the configured Material Parameter Collection. */
    void WriteRadiusToMPC(int32 Radius);
};
