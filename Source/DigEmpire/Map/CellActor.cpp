#include "CellActor.h"

#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"

ACellActor::ACellActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    Root->SetMobility(EComponentMobility::Static);

    CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));
    CellMesh->SetupAttachment(Root);
    CellMesh->SetMobility(EComponentMobility::Static);
    CellMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CellMesh->SetCastShadow(false);
    CellMesh->SetVisibility(false, true); // hidden until seen/in vision
    CellMesh->SetHiddenInGame(true);

    VisionChannel = TAG_Character_Vision_FirstSeen;
}

void ACellActor::BeginPlay()
{
    Super::BeginPlay();
    VisionChannel = TAG_Character_Vision_FirstSeen;

    if (VisionChannel.IsValid())
    {
        UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
        VisionHandle = Bus.RegisterListener<FCellsFirstSeenMessage>(
            VisionChannel,
            [this](FGameplayTag /*Tag*/, const FCellsFirstSeenMessage& Msg)
            {
                HandleVisionMessage(Msg);
            });
    }
}

void ACellActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (VisionHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(VisionHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void ACellActor::HandleVisionMessage(const FCellsFirstSeenMessage& Msg)
{
    // Determine this actor's grid coordinate by world position
    const FVector Loc = GetActorLocation();
    const int32 GX = FMath::RoundToInt(Loc.X / FMath::Max(1.f, TileSize));
    const int32 GY = FMath::RoundToInt(Loc.Y / FMath::Max(1.f, TileSize));

    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        if (Entry.Coord.X == GX && Entry.Coord.Y == GY)
        {
            if (CellMesh)
            {
                CellMesh->SetVisibility(true, true);
                CellMesh->SetHiddenInGame(!true);
            }
            // Blueprint hook
            OnVisionVisibilityChanged(true);
            break;
        }
    }
}
