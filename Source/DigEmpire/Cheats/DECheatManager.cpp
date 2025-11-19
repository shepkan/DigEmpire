#include "DECheatManager.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DigEmpire/Character/CharacterGridVisionComponent.h"

void UDECheatManager::SetVisionRadiusCells(int32 NewRadius)
{
    APlayerController* PC = GetOuterAPlayerController();
    if (!PC)
    {
        return;
    }

    APawn* Pawn = PC->GetPawn();
    if (!Pawn)
    {
        return;
    }

    if (UCharacterGridVisionComponent* Vision = Pawn->FindComponentByClass<UCharacterGridVisionComponent>())
    {
        Vision->VisionRadiusCells = FMath::Max(0, NewRadius);
    }
}

