#include "DigEmpire/Player/DigEmpirePlayerController.h"

#include "DECheatManager.h"

ADigEmpirePlayerController::ADigEmpirePlayerController()
{
    CheatClass = UDECheatManager::StaticClass();
}

