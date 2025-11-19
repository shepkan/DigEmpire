#include "DigEmpire/Player/DigEmpirePlayerController.h"

#include "DigEmpire/Cheats/DECheatManager.h"

ADigEmpirePlayerController::ADigEmpirePlayerController()
{
    CheatClass = UDECheatManager::StaticClass();
}

