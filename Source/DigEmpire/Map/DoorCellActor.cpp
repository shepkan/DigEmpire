#include "DoorCellActor.h"

void ADoorCellActor::OpenDoor()
{
    if (bIsOpen)
    {
        return;
    }
    bIsOpen = true;
    OnDoorOpened();
}

void ADoorCellActor::SetDoorColor(const FGameplayTag& InColor)
{
    DoorColor = InColor;
    OnDoorColorAssigned(InColor);
}
