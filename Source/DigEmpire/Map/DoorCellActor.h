#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CellActor.h"
#include "DoorCellActor.generated.h"

/**
 * Door cell actor that can block movement until opened.
 * Uses DoorColor tag to group doors; opening makes it passable and fires a Blueprint event.
 */
UCLASS(Blueprintable)
class ADoorCellActor : public ACellActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
    FGameplayTag DoorColor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
    bool bIsOpen = false;

    UFUNCTION(BlueprintCallable, Category="Door")
    virtual void OpenDoor();

    UFUNCTION(BlueprintPure, Category="Door")
    bool IsOpen() const { return bIsOpen; }

    virtual bool IsBlocked() const override { return !bIsOpen; }

    UFUNCTION(BlueprintNativeEvent, Category="Door|Events")
    void OnDoorOpened();
    virtual void OnDoorOpened_Implementation() {}
};

