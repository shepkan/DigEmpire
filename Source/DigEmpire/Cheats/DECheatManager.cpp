#include "DECheatManager.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DigEmpire/Character/CharacterGridVisionComponent.h"
#include "DigEmpire/Character/GridMovementComponent.h"

void UDECheatManager::Cheat_SetVisionRadiusCells(int32 NewRadius)
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

void UDECheatManager::Cheat_SetMaxSpeed(float NewMaxSpeed)
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

    if (UGridMovementComponent* Move = Pawn->FindComponentByClass<UGridMovementComponent>())
    {
        Move->MaxSpeed = FMath::Max(0.f, NewMaxSpeed);
    }
}

void UDECheatManager::Cheat_GodMode()
{
    // Pump speed and vision to high values for testing.
    Cheat_SetMaxSpeed(800.f);
    Cheat_SetVisionRadiusCells(100);
}
