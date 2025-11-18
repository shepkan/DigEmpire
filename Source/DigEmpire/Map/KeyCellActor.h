#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CellActor.h"
#include "KeyCellActor.generated.h"

/**
 * Key actor placed on a map cell. When used, it destroys itself and
 * opens all doors with the same DoorColor tag (by calling OpenDoor()).
 */
UCLASS(Blueprintable)
class AKeyCellActor : public ACellActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Key")
    FGameplayTag DoorColor;

    /** Call from Blueprint to consume the key and open matching doors. */
    UFUNCTION(BlueprintCallable, Category="Key")
    void UseKey();
};

