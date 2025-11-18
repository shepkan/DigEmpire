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

