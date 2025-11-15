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

    /** Called when the cell containing this actor becomes visible (first seen). */
    UFUNCTION(BlueprintNativeEvent, Category="CellActor|Events")
    void OnCellSeen();
    virtual void OnCellSeen_Implementation() {}
};
