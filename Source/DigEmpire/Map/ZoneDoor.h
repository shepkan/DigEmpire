#pragma once

#include "CoreMinimal.h"
#include "CellActor.h"
#include "ZoneDoor.generated.h"

/**
 * Door actor placed inside zone passages (tunnels).
 * Blocks movement by default.
 */
UCLASS(Blueprintable)
class AZoneDoor : public ACellActor
{
    GENERATED_BODY()

public:
    virtual bool IsBlocked() const override { return true; }
};

