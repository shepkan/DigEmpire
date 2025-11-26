#include "DigEmpire/Player/DigEmpirePlayerController.h"

#include "DigEmpire/Cheats/DECheatManager.h"
#include "DigEmpire/Inventory/DEInventoryComponent.h"

ADigEmpirePlayerController::ADigEmpirePlayerController()
{
    CheatClass = UDECheatManager::StaticClass();
    Inventory = CreateDefaultSubobject<UDEInventoryComponent>(TEXT("Inventory"));
}
