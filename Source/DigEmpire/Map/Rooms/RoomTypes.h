#pragma once

#include "CoreMinimal.h"
#include "RoomTypes.generated.h"

/** Immutable info about a generated room. */
USTRUCT(BlueprintType)
struct FRoomInfo
{
    GENERATED_BODY()

    /** Zone id the room was placed in. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
    int32 ZoneId = -1;

    /** Top-left cell of the room rectangle. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
    FIntPoint TopLeft = FIntPoint::ZeroValue;

    /** Width,Height of the room in cells. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
    FIntPoint Size = FIntPoint::ZeroValue;

    /** Entrance cell (on the room border, kept open). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room")
    FIntPoint Entrance = FIntPoint::ZeroValue;
};

