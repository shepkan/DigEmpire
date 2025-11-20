#include "DECheatManager.h"

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DigEmpire/Character/CharacterGridVisionComponent.h"
#include "DigEmpire/Character/GridMovementComponent.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "DigEmpire/Map/MapGrid2DComponent.h"
#include "DigEmpire/Map/MapSpriteRenderer.h"

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

void UDECheatManager::Cheat_NextCaveGenerationStep()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Try to get Map component from GameMode; fallback to any in the world
    UMapGrid2DComponent* MapComp = nullptr;
    if (AGameModeBase* GM = World->GetAuthGameMode<AGameModeBase>())
    {
        MapComp = GM->FindComponentByClass<UMapGrid2DComponent>();
    }
    if (!MapComp)
    {
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            if (UMapGrid2DComponent* C = It->FindComponentByClass<UMapGrid2DComponent>())
            {
                MapComp = C; break;
            }
        }
    }
    if (!MapComp) return;

    // Execute next generation step
    MapComp->ExecuteNextGenerationStep();

    // Force redraw using any AMapSpriteRenderer on level
    AMapSpriteRenderer* Renderer = nullptr;
    for (TActorIterator<AMapSpriteRenderer> It(World); It; ++It)
    {
        Renderer = *It; break;
    }
    if (Renderer)
    {
        Renderer->RebuildAllFromMap(MapComp);
    }
}
