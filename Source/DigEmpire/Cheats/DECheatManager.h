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
    /** Sets VisionRadiusCells on the local player's UCharacterGridVisionComponent. Usage: Cheat_SetVisionRadiusCells 8 */
    UFUNCTION(Exec)
    void Cheat_SetVisionRadiusCells(int32 NewRadius);

    /** Sets MaxSpeed on the local player's UGridMovementComponent. Usage: Cheat_SetMaxSpeed 600 */
    UFUNCTION(Exec)
    void Cheat_SetMaxSpeed(float NewMaxSpeed);

    /** Convenience macro: pumps speed and vision for testing. Usage: Cheat_GodMode */
    UFUNCTION(Exec)
    void Cheat_GodMode();

    /** Advances map generation by one step and forces full redraw ignoring vision. Usage: Cheat_NextCaveGenerationStep */
    UFUNCTION(Exec)
    void Cheat_NextCaveGenerationStep();
};
