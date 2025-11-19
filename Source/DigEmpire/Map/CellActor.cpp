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

    VisionChannel = TAG_Character_Vision;
}

void ACellActor::BeginPlay()
{
    Super::BeginPlay();
    VisionChannel = TAG_Character_Vision;

    if (VisionChannel.IsValid())
    {
        UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
        VisionHandle = Bus.RegisterListener<FCharacterGridVisionMessage>(
            VisionChannel,
            [this](FGameplayTag /*Tag*/, const FCharacterGridVisionMessage& Msg)
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

void ACellActor::HandleVisionMessage(const FCharacterGridVisionMessage& Msg)
{
    // Determine this actor's grid coordinate by world position
    const FVector Loc = GetActorLocation();
    const int32 GX = FMath::RoundToInt(Loc.X / FMath::Max(1.f, TileSize));
    const int32 GY = FMath::RoundToInt(Loc.Y / FMath::Max(1.f, TileSize));

    bool bInVisionNow = false;
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        if (Entry.Coord.X == GX && Entry.Coord.Y == GY)
        {
            bInVisionNow = true;
            break;
        }
    }

    if (bInVisionNow != bCurrentlyVisible)
    {
        bCurrentlyVisible = bInVisionNow;
        if (CellMesh)
        {
            CellMesh->SetVisibility(bCurrentlyVisible, true);
            CellMesh->SetHiddenInGame(!bCurrentlyVisible);
        }
        // Blueprint hook
        OnVisionVisibilityChanged(bCurrentlyVisible);
    }
}
