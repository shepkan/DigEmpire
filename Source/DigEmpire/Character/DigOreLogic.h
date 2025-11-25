#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "DigOreLogic.generated.h"

class UMapGrid2DComponent;

/**
 * Base class for ore-destruction logic executed by UDigComponent.
 * Create Blueprint or C++ subclasses and map them per-ore in settings.
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class UDigOreLogicBase : public UObject
{
    GENERATED_BODY()
public:
    /** Execute behavior when a block with given ore is destroyed. */
    UFUNCTION(BlueprintNativeEvent, Category="Dig|OreLogic")
    void Execute(AActor* InstigatorActor, UMapGrid2DComponent* Map, int32 X, int32 Y, FGameplayTag OreTag);
    virtual void Execute_Implementation(AActor* /*InstigatorActor*/, UMapGrid2DComponent* /*Map*/, int32 /*X*/, int32 /*Y*/, FGameplayTag /*OreTag*/) {}
};

/**
 * Adds light power on destruction based on configured amount.
 */
UCLASS(BlueprintType, EditInlineNew)
class UDigOreLogic_AddLight : public UDigOreLogicBase
{
    GENERATED_BODY()
public:
    /** Amount to add to LightPower [can be negative]. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Light", meta=(ClampMin="-100", ClampMax="100"))
    float Amount = 5.f;

    virtual void Execute_Implementation(AActor* InstigatorActor, UMapGrid2DComponent* Map, int32 X, int32 Y, FGameplayTag OreTag) override;
};

