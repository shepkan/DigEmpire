#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomGenSettings.generated.h"

/** Desired room specification. */
USTRUCT(BlueprintType)
struct FRoomSpec
{
    GENERATED_BODY()

    /** Optional fixed zone id for this room; -1 = auto-pick any suitable zone. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room")
    int32 ZoneId = -1;

    /** Room width in cells. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room", meta=(ClampMin="1"))
    int32 Width = 6;

    /** Room height in cells. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room", meta=(ClampMin="1"))
    int32 Height = 6;
};

/** Data asset that contains room requests. */
UCLASS(BlueprintType)
class URoomGenSettings : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Rooms to place within zones. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rooms")
    TArray<FRoomSpec> Rooms;
};

