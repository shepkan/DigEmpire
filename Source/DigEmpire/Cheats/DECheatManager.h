#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "DECheatManager.generated.h"

class UCharacterGridVisionComponent;

/** Project cheat manager: console cheats for runtime tweaking. */
UCLASS()
class UDECheatManager : public UCheatManager
{
    GENERATED_BODY()
public:
    /** Sets VisionRadiusCells on the local player's UCharacterGridVisionComponent. Usage: SetVisionRadiusCells 8 */
    UFUNCTION(Exec)
    void SetVisionRadiusCells(int32 NewRadius);
};

