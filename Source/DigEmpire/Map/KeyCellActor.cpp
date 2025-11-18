#include "KeyCellActor.h"

#include "EngineUtils.h"
#include "DoorCellActor.h"

void AKeyCellActor::UseKey()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // Open all doors in the world that share the same DoorColor
    for (TActorIterator<ADoorCellActor> It(World); It; ++It)
    {
        ADoorCellActor* Door = *It;
        if (!Door) continue;
        if (Door->DoorColor == DoorColor)
        {
            Door->OpenDoor();
        }
    }

    // Destroy the key after use
    Destroy();
}

void AKeyCellActor::SetDoorColor(const FGameplayTag& InColor)
{
    DoorColor = InColor;
    OnDoorColorAssigned(InColor);
}
