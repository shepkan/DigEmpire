#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
    /** Returns true if this actor blocks movement on its cell. */
    UFUNCTION(BlueprintCallable, Category="CellActor")
    virtual bool IsBlocked() const { return false; }
};

